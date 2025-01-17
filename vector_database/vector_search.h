#include <iostream>
#include <fstream>
#include <filesystem>
#include "hnswlib/hnswlib.h"
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

class VectorSearch {
    private:
        hnswlib::HierarchicalNSW<float>* index;
        std::string index_name;

    public:
        VectorSearch(const std::string& index_name, int embedding_size, int word_count) {
            this->index_name = index_name;
            hnswlib::L2Space space(embedding_size);
            index = new hnswlib::HierarchicalNSW<float>(&space, word_count);
        }

        void populate(const std::string& json_directory) {
            // Loop through JSON files in directory
            int id = 0;
            for (const auto& entry : fs::directory_iterator(json_directory)) {
                if (entry.path().extension() == ".json") {
                    std::ifstream file(entry.path());
                    json json_data;
                    file >> json_data;

                    // Insert each record into the index
                    for (const auto& item : json_data) {
                        const std::vector<float> embedding = item["embedding"];
                        index->addPoint(embedding.data(), id);
                        id++;
                    }
                }
            }

            // Serialize index
            index->saveIndex(index_name);
        }

        ~VectorSearch() {
            delete index;
        }
};