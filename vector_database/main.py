import grpc
import service_pb2_grpc
import sys
import os
from service_impl import VectorDatabaseServiceImpl
from concurrent import futures

def serve():
    database_name = sys.argv[1]
    index_name = sys.argv[2]
    embedding_size = int(sys.argv[3])
    word_count = int(sys.argv[4])

    service = VectorDatabaseServiceImpl(database_name, index_name, embedding_size, word_count)
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=1))
    service_pb2_grpc.add_VectorDatabaseServiceServicer_to_server(service, server)

    port = os.getenv("GRPC_PORT", "50051")
    server.add_insecure_port(f"0.0.0.0:{port}")
    server.start()
    print(f"Server listening on port {port}")
    server.wait_for_termination()

if __name__ == "__main__":
    serve()
