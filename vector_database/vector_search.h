#include <iostream>
#include <fstream>
#include <filesystem>
#include "hnswlib/hnswlib.h"
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

class VectorSearch {
    private:
        hnswlib::SpaceInterface<float>* space;
        hnswlib::HierarchicalNSW<float>* index;
        std::string index_name;

        int embedding_size;
        int word_count;

        // HNSW parameters
        size_t M;            
        size_t ef_construction;  
        size_t ef;           
    public:
        VectorSearch(const std::string& index_name, int embedding_size, int word_count, size_t M = 16, size_t ef_construction = 200, size_t ef = 256) {
            this->index_name = index_name;
            this->embedding_size = embedding_size;
            this->word_count = word_count;
            this->M = M;            
            this->ef_construction = ef_construction;  
            this->ef = ef;
            space = new hnswlib::InnerProductSpace(embedding_size); 
            index = new hnswlib::HierarchicalNSW<float>(space, word_count, M, ef_construction);
            index->setEf(ef);
        }

        void load() {
            index->loadIndex(index_name, space, word_count);
            index->setEf(ef);
        }

        void normalize_vector(const float* data, float* norm_array, int dim) {
            float norm = 0.0f;
            for (int i = 0; i < dim; i++)
                norm += data[i] * data[i];
            norm = 1.0f / (sqrtf(norm) + 1e-30f);
            for (int i = 0; i < dim; i++)
                norm_array[i] = data[i] * norm;
        }

        void populate(const std::string& json_directory) {
            // Loop through JSON files in directory and store all embeddings in contiguous block of memory
            std::vector<float> all_embeddings(word_count * embedding_size);
            int position = 0;
            for (const auto& entry : fs::directory_iterator(json_directory)) {
                if (entry.path().extension() == ".json") {
                    std::ifstream file(entry.path());
                    json json_data;
                    file >> json_data;

                    // Insert each embedding into the contiguous memory block
                    for (const auto& item : json_data) {
                        const std::vector<float> embedding = item["embedding"];
                        std::copy(embedding.begin(), embedding.end(), all_embeddings.begin() + position * embedding_size);

                        position++;

                        if (position % 1000 == 0 || position == word_count) {  
                            std::cout << "Extracted " << (100.0 * position / word_count) << "% of embeddings..." << std::endl;  
                        }
                    }
                }
            }

            // Normalize the embeddings and add to index
            for (int i = 0; i < word_count; i++) {
                std::vector<float> norm_array(embedding_size);
                normalize_vector(all_embeddings.data() + i * embedding_size, norm_array.data(), embedding_size);
                index->addPoint(norm_array.data(), i);

                if (i % 1000 == 0 || i == word_count) {  
                    std::cout << "Added " << (100.0 * i / word_count) << "% of embeddings to index..." << std::endl;  
                } 
            }

            // Serialize index
            index->saveIndex(index_name);
        }

        json search(int id, float threshold, int top_n) {
            // Get embedding for given id
            std::vector<float> embedding = index->getDataByLabel<float>(id);

            // Search for top n similar items
            auto nearest_neighbors = index->searchKnn(embedding.data(), top_n + 1);

            // Filter similar items by cosine similarity threshold
            json result = json::array();
            while (!nearest_neighbors.empty()) {
                auto [distance, neighbor_id] = nearest_neighbors.top();
                nearest_neighbors.pop();

                float similarity = 1.0f - distance;

                // Extract and add to results
                if (similarity >= threshold) {
                    json neighbor;
                    neighbor["id"] = neighbor_id;
                    neighbor["similarity"] = similarity;
                    result.push_back(neighbor);
                }
            }
            return result;
        }

        ~VectorSearch() {
            delete index;
            delete space;
        }
};