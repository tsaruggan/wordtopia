from flask import Flask, request, jsonify
import grpc
import service_pb2
import service_pb2_grpc

app = Flask(__name__)

def vector_database_request(word, top_n):
    channel = grpc.insecure_channel("localhost:50051")
    stub = service_pb2_grpc.VectorDatabaseServiceStub(channel)
    grpc_request = service_pb2.SearchRequest(word=word, top_n=top_n)
    response = stub.SearchSimilarWords(grpc_request)
    return [{"word": r.word, "definition": r.definition, "similarity": r.similarity} for r in response.results]

@app.route("/search", methods=["GET"])
def search():
    word = request.args.get("word")
    top_n = int(request.args.get("top_n", 5))
    results = vector_database_request(word, top_n)
    return jsonify(results)

if __name__ == "__main__":
    app.run(port=8080)
