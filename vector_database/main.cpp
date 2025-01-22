#include <grpc++/grpc++.h>
#include "service.grpc.pb.h"
#include "service_impl.h"

int main() {
    // Initialize gRPC service implementation
    std::string json_directory = "./dataset";
    std::string database_name = "dictionary.db";
    std::string index_name = "index.bin";
    int embedding_size = 1536;
    int word_count = 28032;
    VectorDatabaseServiceImpl service(json_directory, database_name, index_name, embedding_size, word_count);

    // Set up the gRPC server builder
    std::string server_address = "0.0.0.0:50051";
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    // Build and start the gRPC server
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();

    return 0;
}