import hnswlib
import pandas as pd
import pyarrow.parquet as pq
import numpy as np

class VectorSearch:
    def __init__(self, index_name, embedding_size, word_count, M=16, ef_construction=200, ef=256):
        self.index_name = index_name
        self.embedding_size = embedding_size
        self.word_count = word_count
        self.M = M
        self.ef_construction = ef_construction
        self.ef = ef

        self.index = hnswlib.Index(space = 'cosine', dim = embedding_size)
        self.index.init_index(max_elements = word_count, ef_construction=ef_construction, M=M)
        self.index.set_ef(ef)

    def load(self):
        self.index.load_index(self.index_name, max_elements = self.word_count)
        self.index.set_ef(self.ef)

    def populate(self, dataset_name):
        pf = pq.ParquetFile(dataset_name)
        current_id = 0
        for batch in pf.iter_batches(batch_size=1000):
            df = batch.to_pandas()
            embeddings = np.stack(df["embedding"].to_numpy())
            self.index.add_items(embeddings, range(current_id, current_id + len(df)))
            current_id += len(df)
        self.index.save_index(self.index_name)

    def search(self, word_id, top_n):
        embedding = self.index.get_items([word_id])[0]
        labels, distances = self.index.knn_query([embedding], k=top_n + 1)

        index_records = {}
        for label, distance in zip(labels[0], distances[0]):
            similarity = 1.0 if label == word_id else 1.0 - distance
            index_records[label] = {"id": label, "similarity": similarity}
        return index_records