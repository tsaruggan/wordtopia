#include <grpc++/grpc++.h>
#include "service.grpc.pb.h"
#include "service_impl.h"

int main(int argc, char** argv) {
    std::string database_name = argv[1];
    std::string index_name = argv[2];
    int embedding_size = std::stoi(argv[3]);
    int word_count = std::stoi(argv[4]);
    VectorDatabaseServiceImpl service(database_name, index_name, embedding_size, word_count);

    // Set up the gRPC server builder
    std::string port = std::getenv("GRPC_PORT") ? std::getenv("GRPC_PORT") : "50051";
    std::string server_address = "0.0.0.0:" + port;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    // Build and start the gRPC server
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();

    return 0;
}