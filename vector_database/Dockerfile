FROM grpc/cxx:latest

# Set working directory
WORKDIR /vector_database

# Install SQLite from source
RUN curl -L https://www.sqlite.org/2025/sqlite-autoconf-3480000.tar.gz -o sqlite-autoconf-3480000.tar.gz && \
    tar xzf sqlite-autoconf-3480000.tar.gz && \
    cd sqlite-autoconf-3480000 && \
    ./configure --prefix=/usr/local && \
    make -j$(nproc) && \
    make install && \
    cd .. && \
    rm -rf sqlite-autoconf-3480000 sqlite-autoconf-3480000.tar.gz

# Install hnswlib
RUN git clone --branch v0.8.0 https://github.com/nmslib/hnswlib.git /hnswlib

# Install nlohmann/json
RUN mkdir -p /usr/local/include/nlohmann && \
    curl -L https://github.com/nlohmann/json/releases/download/v3.7.3/json.hpp -o /usr/local/include/nlohmann/json.hpp

# Copy the dataset
COPY dataset /vector_database/dataset

# Copy source code
COPY vector_database /vector_database

# Generate Protobuf and gRPC files
COPY service.proto /vector_database
RUN protoc --proto_path=. --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` service.proto

# Compile the main C++ application
RUN g++ -o main main.cpp service.pb.cc service.grpc.pb.cc \
    -I/usr/local/include -I/usr/include -I/hnswlib \
    -L/usr/local/lib -L/usr/lib \
    -lprotobuf -lgrpc++ -lsqlite3 -lstdc++fs -std=c++17

# Set the parameter values
ENV JSON_DIRECTORY=./dataset
ENV DATABASE_NAME=dictionary.db
ENV INDEX_NAME=index.bin
ENV EMBEDDING_SIZE=1536
ENV WORD_COUNT=28032

# Compile and run the db/index initialization script
RUN g++ -o init init.cpp \
    -I/usr/local/include -I/usr/include -I/hnswlib \
    -L/usr/local/lib -L/usr/lib \
    -lsqlite3 -lstdc++fs -std=c++17
RUN ./init $JSON_DIRECTORY $DATABASE_NAME $INDEX_NAME $EMBEDDING_SIZE $WORD_COUNT

# Delete the dataset folder as it's no longer needed
RUN rm -rf /vector_database/dataset

# Persistence
VOLUME ["/vector_database"]

# Expose the necessary port
EXPOSE 50051

# Run the compiled binary
CMD ./main $DATABASE_NAME $INDEX_NAME $EMBEDDING_SIZE $WORD_COUNT
