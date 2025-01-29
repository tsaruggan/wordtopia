from flask import Flask, request, jsonify, send_from_directory
import os
import grpc
import service_pb2
import service_pb2_grpc

app = Flask(__name__, static_folder='dist')

def vector_database_search(word, n):
    grpc_port = int(os.environ.get("GRPC_PORT", 50051))
    channel = grpc.insecure_channel("vector_database:"+ str(grpc_port))
    stub = service_pb2_grpc.VectorDatabaseServiceStub(channel)
    grpc_request = service_pb2.SearchRequest(word=word, n=n)
    response = stub.SearchSimilarWords(grpc_request)
    return [{"word": r.word, "definition": r.definition, "similarity": r.similarity} for r in response.results]

def vector_database_suggest(prefix, n):
    grpc_port = int(os.environ.get("GRPC_PORT", 50051))
    channel = grpc.insecure_channel("vector_database:"+ str(grpc_port))
    stub = service_pb2_grpc.VectorDatabaseServiceStub(channel)
    grpc_request = service_pb2.SuggestionRequest(prefix=prefix, n=n)
    response = stub.SuggestWords(grpc_request)
    return [{"word": r.word, "definition": r.definition} for r in response.suggestions]

@app.route('/')
def index():
    return send_from_directory('dist', 'index.html')

@app.route('/<path:path>')
def static_files(path):
    return send_from_directory('dist', path)

@app.route("/search", methods=["GET"])
def search():
    word = request.args.get("word")
    n = int(request.args.get("n", 5))
    results = vector_database_search(word, n)
    return jsonify(results)

@app.route("/suggest", methods=["GET"])
def suggest():
    prefix = request.args.get("prefix")
    n = int(request.args.get("n", 8))
    suggestions = vector_database_suggest(prefix, n)
    return jsonify(suggestions)

if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8080))
    app.run(host="0.0.0.0", port=port)
