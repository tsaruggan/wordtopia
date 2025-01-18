#include <iostream>
#include <fstream>
#include <filesystem>
#include <sqlite3.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

// Custom exception for words that do not exist
class WordNotFoundException : public std::exception {
    private:
        std::string message;

    public:
        explicit WordNotFoundException(const std::string& word) {
            message = "The word '" + word + "' does not exist in the dictionary.";
        }

        const char* what() const noexcept override {
            return message.c_str();
        }
};

// Custom exception for database query preparation failures ex. locking, resource limits, connection issue
class DatabaseFailureException : public std::exception {
    private:
        std::string message;

    public:
        explicit DatabaseFailureException(const std::string& error_message) {
            message = "Database failure: " + error_message;
        }

        const char* what() const noexcept override {
            return message.c_str();
        }
};

// Class for interfacing (populating, querying) with SQLite database
class Database {
    private:
        sqlite3 *db;

    public:
        Database(std::string database_name) {
            sqlite3_open(database_name.c_str(), &db);
        }

        void populate(const std::string& json_directory) {
            // Create table
            std::string table_name = "dictionary";
            std::string create_table_sql = "CREATE TABLE IF NOT EXISTS dictionary (id INTEGER PRIMARY KEY AUTOINCREMENT, word TEXT, definition TEXT);";
            // std::string create_table_sql = "CREATE TABLE IF NOT EXISTS dictionary (id INTEGER PRIMARY KEY AUTOINCREMENT, word TEXT);";
            sqlite3_exec(db, create_table_sql.c_str(), 0, 0, 0);

            // Loop through JSON files in directory
            for (const auto& entry : fs::directory_iterator(json_directory)) {
                if (entry.path().extension() == ".json") {
                    std::ifstream file(entry.path());
                    json json_data;
                    file >> json_data;

                    // Insert each record into the database
                    for (const auto& item : json_data) {
                        std::string word = item["word"];
                        std::string definition = item["definition"];
                        std::string insert_sql = "INSERT INTO dictionary (word, definition) VALUES ('" + word + "', '" + definition + "');";
                        // std::string insert_sql = "INSERT INTO dictionary (word) VALUES ('" + word + "');";
                        sqlite3_exec(db, insert_sql.c_str(), 0, 0, 0);
                    }
                }
            }
        }

        // Check if a word exists in the database
       bool word_exists(const std::string& word) {
            // If query preparation fails, throw an exception
            std::string query_sql = "SELECT COUNT(*) FROM dictionary WHERE word = ?;";
            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(db, query_sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                std::string error = sqlite3_errmsg(db);
                throw DatabaseFailureException(error);
            }

            // Bind word to query to prevent injection attack
            sqlite3_bind_text(stmt, 1, word.c_str(), -1, SQLITE_STATIC); 

            // Execute statement and check if the word exists
            bool exists = false;
            if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
                exists = true;
            }

            // Clean up resources & return
            sqlite3_finalize(stmt); 
            return exists;
        }

        // Fetch the id associated with the word
        int get_id(const std::string& word) {
            // If word does not exist, throw an exception
            if (!word_exists(word)) {
                throw WordNotFoundException(word);
            }

            // If query preparation fails, throw an exception
            std::string query_sql = "SELECT id FROM dictionary WHERE word = ?;";
            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(db, query_sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                std::string error = sqlite3_errmsg(db);
                throw DatabaseFailureException(error);
            }

            // Bind word to query to prevent injection attack
            sqlite3_bind_text(stmt, 1, word.c_str(), -1, SQLITE_STATIC); 

            // Execute statement and extract id
            int id = -1;
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                id = sqlite3_column_int(stmt, 0);
            }

            // Clean up resources & return
            sqlite3_finalize(stmt);
            return id;
        }

        // Fetch the full database records associated with a set of ids
        json get_dictionary_records(const std::vector<int>& ids) {
            // Construct batch query by directly embedding each id into the query string
            std::string query_sql = "SELECT id, word, definition FROM dictionary WHERE id IN (";
            // std::string query_sql = "SELECT id, word FROM dictionary WHERE id IN (";
            for (size_t i = 0; i < ids.size(); ++i) {
                query_sql += std::to_string(ids[i]);
                if (i < ids.size() - 1) {
                    query_sql += ", ";
                }
            }
            query_sql += ");";

            // If query preparation fails, throw an exception
            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(db, query_sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                std::string error = sqlite3_errmsg(db);
                throw DatabaseFailureException(error);
            }

            // Execute statement and extract columns to JSON
            json results = json::array();
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                json record;
                record["id"] = sqlite3_column_int(stmt, 0);
                record["word"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                record["definition"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                results.push_back(record);
            }
            
            // Clean up resources & return
            sqlite3_finalize(stmt);
            return results;
        }

        ~Database() {
            sqlite3_close(db);
        }
};