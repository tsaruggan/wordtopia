import sys
from database import Database
from vector_search import VectorSearch

dataset_name = sys.argv[1]
database_name = sys.argv[2]
index_name = sys.argv[3]
embedding_size = int(sys.argv[4])
word_count = int(sys.argv[5])

database = Database(database_name)
database.populate(dataset_name)

vector_search = VectorSearch(index_name, embedding_size, word_count, 16, 200, 10)
vector_search.populate(dataset_name)
