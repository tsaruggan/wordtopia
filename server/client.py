import grpc
import service_pb2
import service_pb2_grpc

def search(word, top_n):
    # Connect to the C++ gRPC server
    channel = grpc.insecure_channel("localhost:50051")
    stub = service_pb2_grpc.VectorDatabaseServiceStub(channel)

    # Send the request
    request = service_pb2.SearchRequest(word=word, top_n=top_n)
    response = stub.SearchSimilarWords(request)

    # Print results
    for result in response.results:
        print()
        print(f"Word: {result.word}, Definition: {result.definition}, Similarity: {result.similarity}")

if __name__ == "__main__":
    search("abalone", 5)
