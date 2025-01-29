# Use an official Python runtime as a parent image
FROM python:3.9-slim

# Set the working directory
WORKDIR /server

# Copy the entire project
COPY server /server

# Install required Python packages
RUN pip install --no-cache-dir flask flask-cors grpcio grpcio-tools

# Generate gRPC code
COPY service.proto /server
RUN python3 -m grpc_tools.protoc -I=. --python_out=. --grpc_python_out=. service.proto


# # Install curl
# RUN apt-get update && apt-get install -y curl

# # Install Node.js and npm
# RUN curl -fsSL https://deb.nodesource.com/setup_23.x -o nodesource_setup.sh \
#     && bash nodesource_setup.sh \
#     && apt-get install -y nodejs \
#     && apt-get clean

# # Copy the React app and build
# COPY app /app
# WORKDIR /app
# RUN npm install && npm run build

# # Move the production build to the server folder
# RUN cp -r /app/dist /server/dist

# Set the working directory back to the server
WORKDIR /server

# Expose the port your Flask app runs on
# EXPOSE 8080

# Run the application
CMD ["python3", "server.py"]
