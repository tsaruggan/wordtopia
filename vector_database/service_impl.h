#include <grpc++/grpc++.h>
#include "service.grpc.pb.h"
#include "database.h"
#include "vector_search.h"
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class VectorDatabaseServiceImpl final : public service::VectorDatabaseService::Service {
    private:
        Database database;
        VectorSearch vector_search;

    public:
        VectorDatabaseServiceImpl(const std::string& database_name, const std::string& index_name, int embedding_size, int word_count)
            : database(database_name), vector_search(index_name, embedding_size, word_count, 16, 200, 10) {
            vector_search.load();
        }

        grpc::Status SearchSimilarWords(grpc::ServerContext* context, const service::SearchRequest* request, service::SearchResponse* response) override {
            std::string word = request->word();
            int n = request->n();
            
            // Get the id for the word
            int id = database.get_id(word);
            
            // Search for similar words using id
            json search_results = vector_search.search(id, n);

            // Extract IDs from search results
            std::vector<int> ids = { id };
            for (const auto& item : search_results) {
                ids.push_back(item["id"]);
            }

            // Fetch dictionary records for similar words
            json dictionary_records = database.get_dictionary_records(ids);
            
            // Merge dictionary records with similarity scores
            for (const auto& record : dictionary_records) {
                service::SearchResult* result = response->add_results();
                int id = record["id"];
                
                // Find corresponding similarity score
                float similarity = 1.0;
                for (const auto& item : search_results) {
                    if (item["id"] == id) {
                        similarity = item["similarity"];
                        break;
                    }
                }

                // Merge vector search and dictionary props in result
                result->set_word(record["word"]);
                result->set_definition(record["definition"]);
                result->set_similarity(similarity);
            }
            
            return grpc::Status::OK;
        }

        grpc::Status SuggestWords(grpc::ServerContext* context, const service::SuggestionRequest* request, service::SuggestionResponse* response) override {
            std::string prefix = request->prefix();
            int n = request->n();

            // Query database for word suggestions
            json suggestions = database.suggest(prefix, n);

            // Populate gRPC response
            for (const auto& suggestion : suggestions) {
                service::SuggestionResult* result = response->add_suggestions();
                result->set_word(suggestion["word"]);
                result->set_definition(suggestion["definition"]);
            }

            return grpc::Status::OK;
        }

};
