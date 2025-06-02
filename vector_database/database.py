import pandas as pd
import pyarrow.parquet as pq
import sqlite3

class WordNotFoundException(Exception):
    def __init__(self, word):
        self.message = f"The word '{word}' does not exist in the dictionary."
        super().__init__(self.message)

class DatabaseFailureException(Exception):
    def __init__(self, error_message):
        self.message = f"Database failure: {error_message}"
        super().__init__(self.message)

class Database:
    def __init__(self, database_name):
        self.db = sqlite3.connect(database_name, check_same_thread=False)
        self.db.row_factory = sqlite3.Row
        
    def populate(self, dataset_name):
        pf = pq.ParquetFile(dataset_name)
        current_id = 0
        for batch in pf.iter_batches(batch_size=1000):
            df = batch.to_pandas()
            df = df[["word", "definition"]]
            df.insert(0, "id", range(current_id, current_id + len(df)))
            df.to_sql("dictionary", self.db, if_exists="append", index=False)
            current_id += len(df)

    def word_exists(self, word):
        try:
            query = "SELECT 1 FROM dictionary WHERE word = ? LIMIT 1"
            cursor = self.db.execute(query, (word,))
            exists = cursor.fetchone() is not None
            return exists
        except Exception as e:
            raise DatabaseFailureException(str(e))

    def get_id(self, word):
        if not self.word_exists(word):
            raise WordNotFoundException(word)

        try:
            query = "SELECT id FROM dictionary WHERE word = ?"
            cursor = self.db.execute(query, (word,))
            result = cursor.fetchone()
            word_id = result["id"]
            return word_id
        except Exception as e:
            raise DatabaseFailureException(str(e))

    def get_dictionary_records(self, ids):
        try:
            ids_str = ",".join(str(int(i)) for i in ids)
            query = f"SELECT id, word, definition FROM dictionary WHERE id IN ({ids_str})"
            cursor = self.db.execute(query)
            result = cursor.fetchall()
            records = [dict(row) for row in result]
            dictionary_records = {item["id"]: item for item in records}
            return dictionary_records
        except Exception as e:
            raise DatabaseFailureException(str(e))

    def suggest(self, prefix, n):
        try:
            query = "SELECT word, definition FROM dictionary WHERE word LIKE ? LIMIT ?"
            cursor = self.db.execute(query, (prefix + "%", n))
            result = cursor.fetchall()
            suggestions = [dict(row) for row in result]
            return suggestions
        except Exception as e:
            raise DatabaseFailureException(str(e))