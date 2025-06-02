import service_pb2
import service_pb2_grpc
from database import Database
from vector_search import VectorSearch

class VectorDatabaseServiceImpl(service_pb2_grpc.VectorDatabaseServiceServicer):
    def __init__(self, database_name, index_name, embedding_size, word_count):
        self.database = Database(database_name)
        self.vector_search = VectorSearch(index_name, embedding_size, word_count, 16, 200, 10)
        self.vector_search.load()

    def SearchSimilarWords(self, request, context):
        word = request.word
        n = request.n
        print("Received Search Request for word:", word, "with n =", n)

        word_id = self.database.get_id(word)
        print("Word ID:", word_id)
        index_records = self.vector_search.search(word_id, n)
        print("Index Records:", index_records)
        ids = index_records.keys()
        dictionary_records = self.database.get_dictionary_records(ids)
        print("Dictionary Records:", dictionary_records)

        response = service_pb2.SearchResponse()
        for id in ids:
            index_record = index_records[id]
            dictionary_record = dictionary_records[id]

            result = response.results.add()
            result.word = dictionary_record['word']
            result.definition = dictionary_record['definition']
            result.similarity = index_record['similarity']
        return response

    def SuggestWords(self, request, context):
        prefix = request.prefix
        n = request.n

        suggestions = self.database.suggest(prefix, n)

        response = service_pb2.SuggestionResponse()
        for suggestion in suggestions:
            result = response.suggestions.add()
            result.word = suggestion['word']
            result.definition = suggestion['definition']
        return response
