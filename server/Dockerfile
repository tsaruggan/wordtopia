# Use an official Python runtime as a parent image
FROM python:3.9-slim

# Set the working directory
WORKDIR /server

# Copy the entire project
COPY server /server

# Install required Python packages
RUN pip install --no-cache-dir flask grpcio grpcio-tools

# Generate gRPC code
COPY protobuf/service.proto /server
RUN python3 -m grpc_tools.protoc -I=. --python_out=. --grpc_python_out=. service.proto

# Expose the port your Flask app runs on
EXPOSE 8080

# Run the application
CMD ["python3", "server.py"]
