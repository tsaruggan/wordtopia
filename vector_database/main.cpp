#include <iostream>
#include <fstream>
#include <filesystem>
#include <sqlite3.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

int main() {
    // Create the SQLite database
    sqlite3 *db;
    sqlite3_open("dictionary.db", &db);

    // Create the dictionary table
    const char* create_table_sql = "CREATE TABLE dictionary (word TEXT PRIMARY KEY, definition TEXT);";
    sqlite3_exec(db, create_table_sql, 0, 0, 0);

    // Loop through files in the "dictionary" folder
    for (const auto& entry : fs::directory_iterator("../dictionary")) {
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

    // Close the database
    sqlite3_close(db);  
    return 0;
}

// g++ -o main main.cpp -lsqlite3 -std=c++17