#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <sqlite3.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::experimental::filesystem;

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
            std::cout << "Populating database from " << json_directory << "..." << std::endl;

            // Create table with id (starting from 0), word, and definition
            std::string create_table_sql = "CREATE TABLE IF NOT EXISTS dictionary (id INTEGER PRIMARY KEY, word TEXT, definition TEXT);";
            sqlite3_exec(db, create_table_sql.c_str(), 0, 0, 0);

            // Create index to support prefix suggestion search on words
            std::string create_index_sql = "CREATE INDEX IF NOT EXISTS suggest ON dictionary(word);";
            sqlite3_exec(db, create_index_sql.c_str(), 0, 0, 0);

            // Sort JSON dataset files in alphabetical order
            std::vector<fs::path> data_files;
            for (const auto& entry : fs::directory_iterator(json_directory)) {
                if (entry.path().extension() == ".json") {
                    data_files.push_back(entry.path());
                }
            }
            std::sort(data_files.begin(), data_files.end());

            // Loop through JSON files, extract data, and populate database
            int id = 0;
            for (const auto& path : data_files) {
                std::cout << "Extracting data from " << path << "..." << std::endl;

                std::ifstream file(path);
                json json_data;
                file >> json_data;
                
                // Insert each record into the database
                for (const auto& item : json_data) {
                    std::string word = item["word"];
                    std::string definition = item["definition"];

                    // Use prepared statement to prevent SQL injection
                    std::string insert_sql = "INSERT INTO dictionary (id, word, definition) VALUES (?, ?, ?);";
                    sqlite3_stmt* stmt;
                    if (sqlite3_prepare_v2(db, insert_sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                        sqlite3_bind_int(stmt, 1, id);
                        sqlite3_bind_text(stmt, 2, word.c_str(), -1, SQLITE_STATIC);
                        sqlite3_bind_text(stmt, 3, definition.c_str(), -1, SQLITE_STATIC);
                        sqlite3_step(stmt);
                        sqlite3_finalize(stmt);
                    }
                    id++;
                }
            }

            std::cout << "Dictionary seeding complete!" << std::endl;
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

        // Fetch the word & defintions of suggested words for a given prefix
        json suggest(const std::string& prefix, int n) {
            // If query preparation fails, throw an exception
            std::string query_sql = "SELECT word, definition FROM dictionary WHERE word LIKE ? LIMIT ?;";
            sqlite3_stmt *stmt;
            if (sqlite3_prepare_v2(db, query_sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
                std::string error = sqlite3_errmsg(db);
                throw DatabaseFailureException(error);
            }

            // Bind prefix to query to prevent injection attack
            std::string prefix_pattern = prefix + "%";
            sqlite3_bind_text(stmt, 1, prefix_pattern.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, n);

            // Execute statement and extract columns to JSON
            json results = json::array();
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                json record;
                record["word"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                record["definition"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
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