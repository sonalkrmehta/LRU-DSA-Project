Advanced LRU Cache Simulator

A professional-grade C++ simulator for the Least Recently Used (LRU) caching algorithm. This project demonstrates high-performance data structure implementation combined with a visually appealing terminal-based User Interface.

🚀 Key Features

O(1) Efficiency: Both GET and PUT operations run in constant time.

Real-time Visualization: See exactly how nodes move from MRU to LRU positions.

Modern CLI UI: Uses ANSI escape codes for a "Dark Mode" experience.

Dynamic Stats: Tracks hits, misses, and evictions live.

Logging: Every interaction is recorded in cache_log.txt.

🐳 Running with Docker

You can run this simulator inside a Docker container to ensure a consistent environment.

1. Build the Image

docker build -t lru-simulator .


2. Run the Container

Since this is an interactive CLI app, you must use the -it flags:

docker run -it --name lru-app lru-simulator


3. Persisting Logs (Optional)

If you want to view the cache_log.txt on your host machine:

docker run -it -v $(pwd)/cache_log.txt:/app/cache_log.txt lru-simulator


🛠️ Manual Compilation

Prerequisites:

A C++ compiler supporting C++17.

Compilation:

g++ -std=c++17 lru_cache_simulator.cpp -o lru_sim


Execution:

./lru_sim


Created for DSA Visualization and Educational Purposes.


# Sonal Kumar
