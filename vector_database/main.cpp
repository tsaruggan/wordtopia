#include "database.h"
#include "vector_search.h"

int main() {
    int embedding_size = 300;
    int word_count = 28032;
    // int word_count = 28;
    std::string json_directory = "../dictionary";
    std::string database_name = "dictionary.db";
    std::string index_name = "index.bin";

    // Initialize database
    Database database(database_name);
    database.populate(json_directory);

    // Initialize vector search
    VectorSearch vector_search(index_name, embedding_size, word_count);
    vector_search.populate(json_directory);
    // vector_search.load();

    // Example word
    std::string word = "abalone";
    
    // Get the id for the word
    int id = database.get_id(word);
    std::cout << "word: '" << word << "'  -> id: " << id << std::endl;
    
    // Search for similar words
    float threshold = 0.0;  // Adjust as needed
    int top_n = 6;
    json similar_ids = vector_search.search(id, threshold, top_n);

    // Extract IDs from search results
    std::vector<int> ids;
    for (const auto& item : similar_ids) {
        ids.push_back(item["id"]);
    }

    // Fetch dictionary records for similar words
    json records = database.get_dictionary_records(ids);

    // Print the resulting JSON
    // std::cout << "Similar words: " << records.dump(4) << std::endl;
    std::cout << "Similarity: " << similar_ids.dump(4) << std::endl;

    // Print the words
    for (const auto& record : records) {
        std::cout << record["word"] << std::endl;
    }


    return 0;
}

// g++ -o main main.cpp -I./hnswlib -lsqlite3 -std=c++17
