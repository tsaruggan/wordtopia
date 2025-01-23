#include "database.h"
#include "vector_search.h"

int main(int argc, char** argv) {
    // std::string json_directory = "./dataset";
    // std::string database_name = "dictionary.db";
    // std::string index_name = "index.bin";
    // int embedding_size = 1536;
    // int word_count = 28032;

    std::string json_directory = argv[1];
    std::string database_name = argv[2];
    std::string index_name = argv[3];
    int embedding_size = std::stoi(argv[4]);
    int word_count = std::stoi(argv[5]);

    Database database(database_name);
    database.populate(json_directory);

    VectorSearch vector_search(index_name, embedding_size, word_count, 16, 200, 10);
    vector_search.populate(json_directory);

    return 0;
}