# Pull the latest GCC image from Docker Hub
FROM gcc:latest
# Set the working directory inside the container
WORKDIR /app

COPY lru_cache_simulator.cpp .

COPY README.md .

RUN g++ -std=c++17 lru_cache_simulator.cpp -o lru_sim

RUN touch cache_log.txt && chmod 666 cache_log.txt

CMD ["./lru_sim"]
