# Use a lightweight GCC image
FROM gcc:latest

# Set the working directory in the container
WORKDIR /app

# Copy the source code and README into the container
COPY lru_cache_simulator.cpp .
COPY README.md .

# Compile the C++ code with C++17 support
RUN g++ -std=c++17 lru_cache_simulator.cpp -o lru_sim

# Create an empty log file to ensure permissions are correct
RUN touch cache_log.txt && chmod 666 cache_log.txt

# Run the simulator in an interactive terminal mode
# Note: You must run the container with -it flags to see the UI correctly
CMD ["./lru_sim"]
