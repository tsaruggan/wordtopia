#include "database.h"
#include "vector_search.h"
#include "hnswlib/hnswlib.h"

int main() {
    int embedding_size = 300;
    int word_count = 28032;
    std::string json_directory = "../dictionary";
    std::string database_name = "dictionary.db";
    std::string table_name = "dictionary";
    std::string index_name = "index.bin";
    std::vector<std::string> database_columns = {"word", "definition"};
    std::string embedding_column = "embedding";

    // Initialize database
    Database db(database_name);
    db.populate(json_directory, table_name, database_columns);

    // Initialize vector search
    VectorSearch vector_search(index_name, embedding_size, word_count);
    vector_search.populate(json_directory, embedding_column);

    return 0;
}

// g++ -o main main.cpp -I./hnswlib -lsqlite3 -std=c++17
