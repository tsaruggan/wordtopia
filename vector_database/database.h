#include <iostream>
#include <fstream>
#include <filesystem>
#include <sqlite3.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

class Database {
    private:
        sqlite3 *db;

    public:
        Database(std::string database_name) {
            sqlite3_open(database_name.c_str(), &db);
        }

        void populate(std::string json_directory, std::string table_name) {
            // Create the dictionary table
            const std::string create_table_sql = "CREATE TABLE " + table_name + " (word TEXT PRIMARY KEY, definition TEXT);";
            sqlite3_exec(db, create_table_sql.c_str(), 0, 0, 0);

            // Loop through files in the "dictionary" folder
            for (const auto& entry : fs::directory_iterator(json_directory)) {
                if (entry.path().extension() == ".json") {
                    // Open each JSON file and parse JSON
                    std::ifstream file(entry.path());  
                    json data;
                    file >> data;

                    // Insert each record into database
                    for (const auto& item : data) {
                        const std::string word = item["word"];
                        const std::string definition = item["definition"];
                        const std::string insert_sql = "INSERT INTO dictionary (word, definition) VALUES ('" + word + "', '" + definition + "');";
                        sqlite3_exec(db, insert_sql.c_str(), 0, 0, 0);
                    }
                }
            }
        }

        void close() {
            // Close the database
            sqlite3_close(db);  
        }
};