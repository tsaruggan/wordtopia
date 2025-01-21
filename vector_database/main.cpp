#include <filesystem>
#include "database.h"
#include "vector_search.h"

int main() {
    // Dataset details
    std::string json_directory = "../dictionary";
    int embedding_size = 1536;
    int word_count = 28032;

    // Initialize database
    std::string database_name = "dictionary.db";
    Database database(database_name);
    if (!std::filesystem::exists(database_name)) {
        database.populate(json_directory);
    }

    // Initialize vector search
    std::string index_name = "index.bin";
    size_t M = 16;            
    size_t ef_construction = 200;  
    size_t ef = 10; 
    VectorSearch vector_search(index_name, embedding_size, word_count, M, ef_construction, ef);
    if (!std::filesystem::exists(index_name)) {
        vector_search.populate(json_directory);
    } else {
        vector_search.load();
    }

    // Example word
    std::string word = "cancer";
    
    // Get the id for the word
    int id = database.get_id(word);
    std::cout << "Word: \"" << word << "\" [" << id << "]" << std::endl;
    
    // Search for similar words
    int top_n = 5;
    json search_results = vector_search.search(id, top_n);

    // Extract IDs from search results
    std::vector<int> ids = { id };
    for (const auto& item : search_results) {
        ids.push_back(item["id"]);
    }

    // Fetch dictionary records for similar words
    json dictionary_records = database.get_dictionary_records(ids);

    // Merge dictionary records with similarity scores
    json merged_results = json::array();
    for (const auto& record : dictionary_records) {
        int id = record["id"];
        
        // Find corresponding similarity score
        float similarity = 1.0;
        for (const auto& item : search_results) {
            if (item["id"] == id) {
                similarity = item["similarity"];
                break;
            }
        }

        // Add merged record/result
        json merged = record;
        merged["similarity"] = similarity;
        merged_results.push_back(merged);
    }

    // Sort by similarity measure
    std::sort(merged_results.begin(), merged_results.end(), [](const json& a, const json& b) {
        return a["similarity"] > b["similarity"];
    });

    // Print
    std::cout << merged_results.dump(4) << std::endl;

    return 0;
}

// g++ -o main main.cpp -I./hnswlib -lsqlite3 -std=c++17
