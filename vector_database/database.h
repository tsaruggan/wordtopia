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

        void populate(const std::string& json_directory, const std::string& table_name, const std::vector<std::string>& database_columns) {
            // Create table
            std::string create_table_sql = "CREATE TABLE " + table_name + " (id INTEGER PRIMARY KEY AUTOINCREMENT";
            for (size_t i = 1; i < database_columns.size(); ++i) {
                create_table_sql += ", " + database_columns[i] + " TEXT";
            }
            create_table_sql += ");";
            sqlite3_exec(db, create_table_sql.c_str(), 0, 0, 0);

            // Loop through JSON files in directory
            for (const auto& entry : fs::directory_iterator(json_directory)) {
                if (entry.path().extension() == ".json") {
                    std::ifstream file(entry.path());
                    json json_data;
                    file >> json_data;

                    // Insert each record into the database
                    for (const auto& item : json_data) {
                        std::string insert_sql = "INSERT INTO " + table_name + " (";
                        
                        for (size_t i = 1; i < database_columns.size(); ++i) {
                            insert_sql += database_columns[i];
                            if (i != database_columns.size() - 1) {
                                insert_sql += ", ";
                            }
                        }

                        insert_sql += ") VALUES (";
                        
                        for (size_t i = 1; i < database_columns.size(); ++i) {
                            insert_sql += "'" + item[database_columns[i]].get<std::string>() + "'";
                            if (i != database_columns.size() - 1) {
                                insert_sql += ", ";
                            }
                        }

                        insert_sql += ");";
                        sqlite3_exec(db, insert_sql.c_str(), 0, 0, 0);
                    }
                }
            }
        }

        ~Database() {
            sqlite3_close(db);  
            // delete db;
        }
};