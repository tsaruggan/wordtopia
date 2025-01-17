#include "database.h"
#include "vector_search.h"

int main() {
    int embedding_size = 300;
    int word_count = 28032;
    std::string json_directory = "../dictionary";
    std::string database_name = "dictionary.db";
    std::string index_name = "index.bin";

    // Initialize database
    Database database(database_name);
    database.populate(json_directory);

    // Initialize vector search
    VectorSearch vector_search(index_name, embedding_size, word_count);
    vector_search.populate(json_directory);

    // Example word
    std::string word = "apple";
    
    // Get the id for the word
    int id = database.get_id(word);
    std::cout << "word: '" << word << "'  -> id: " << id << std::endl;
    
    // Fetch the dictionary records for the given id(s)
    std::vector<int> ids = {id}; 
    json records = database.get_dictionary_records(ids);
    
    // Print the resulting JSON
    std::cout << "Dictionary: " << records.dump(4) << std::endl;

    return 0;
}

// g++ -o main main.cpp -I./hnswlib -lsqlite3 -std=c++17
