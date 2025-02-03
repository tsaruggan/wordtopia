# Wordtopia 🌐

**[Wordtopia](https://wordtopia.vercel.app)** is an AI dictionary + thesaurus powered by OpenAI language models. Use our custom vector search engine to find similar words.

## About
Language is always evolving, with words changing in meaning over time due to our collective memory, semantic context, and daily use. This fun LLM experiment lets us to study the what AI may internally deduce words to mean, especially in relation to other words in its training corpus. Rather than looking up a hardcoded thesaurus, Wordtopia uses vector search to find words with cosine-similar embeddings (not necessarily synoynms or antoynms). [Word embeddings](https://platform.openai.com/docs/guides/embeddings?lang=python) are vector representations of words where the distance between two vectors measures their relatedness. 

## How it works

By leveraging prompt engineering techniques and OpenAI's Batch API, we generated definitions and embeddings for over 300K words. Next, we use this giant pre-generated dataset to seed our database & search index and integrate it all together into a full-stack web app.

We developed a custom vector database using C++ for fast indexing and cosine-similarity search. This solution encapsulates a SQLite database and [HNSWLib](https://github.com/nmslib/hnswlib) search index. A Flask server exposes a REST API to interface with the database by forwarding autocomplete and search requests via gRPC and Protocol Buffers. This microservice architecture is deployed onto an AWS EC2 instance using Docker Compose for container orchestration. Lastly, an interactive React frontend app hosted on Vercel brings the whole project to life. 


![wordtopia-arch](https://github.com/user-attachments/assets/3e3a03dc-3b07-4ca2-bd35-479be0e38cf5)
