# Use an official Python runtime as a parent image
FROM python:3.9-slim

# Set the working directory
WORKDIR /vector_database

# Install C++ build tools and other dependencies
RUN apt-get update && apt-get install -y g++ build-essential && rm -rf /var/lib/apt/lists/*

# Install required Python packages
RUN pip install --no-cache-dir grpcio grpcio-tools pandas pyarrow hnswlib numpy

# Generate gRPC code
COPY service.proto /vector_database
RUN python3 -m grpc_tools.protoc -I=. --python_out=. --grpc_python_out=. service.proto

# Copy the source code
COPY vector_database /vector_database

# Copy the dataset
COPY data.parquet /vector_database


# Make directory for persistent storage
RUN mkdir /vector_database/data

# Set the parameter values
ENV DATASET_NAME=./data.parquet
ENV DATABASE_NAME=./data/dictionary.db
ENV INDEX_NAME=./data/index.bin
ENV EMBEDDING_SIZE=1536
ENV WORD_COUNT=28032
# ENV WORD_COUNT=28

# Run the db/index initialization script
RUN python3 ./init.py $DATASET_NAME $DATABASE_NAME $INDEX_NAME $EMBEDDING_SIZE $WORD_COUNT

# Start the gRPC service
CMD python3 main.py $DATABASE_NAME $INDEX_NAME $EMBEDDING_SIZE $WORD_COUNT
