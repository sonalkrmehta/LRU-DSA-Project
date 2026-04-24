#include <iostream>
#include <unordered_map>
#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <thread>

 
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DARK_BG "\033[48;5;234m"
#define CARD_BG "\033[48;5;236m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define PURPLE  "\033[35m"
#define WHITE   "\033[37m"
#define CLEAR   "\033[2J\033[H"

/**
 * @class Logger
 * @brief Handles file-based logging with timestamps.
 */
class Logger {
    std::ofstream logFile;
public:
    Logger(const std::string& filename = "cache_log.txt") {
        logFile.open(filename, std::ios::app);
        log("SIMULATOR START", "System initialized.");
    }

    ~Logger() {
        log("SIMULATOR END", "System closed.");
        if (logFile.is_open()) logFile.close();
    }

    void log(const std::string& type, const std::string& message) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string ts = std::ctime(&now);
        ts.pop_back(); // Remove newline

        if (logFile.is_open()) {
            logFile << "[" << ts << "] [" << type << "] " << message << std::endl;
        }
    }
};

/**
 * @class LRUCache
 * @brief Implements the Least Recently Used Cache logic using a Hash Map and Doubly Linked List.
 */
class LRUCache {
private:
    struct Node {
        int key;
        int value;
        Node(int k, int v) : key(k), value(v) {}
    };

    int capacity;
    std::list<Node> cacheList; // Doubly Linked List to maintain order (front = MRU, back = LRU)
    std::unordered_map<int, std::list<Node>::iterator> cacheMap; // O(1) Access
    
    // Stats
    int hits = 0;
    int misses = 0;
    int evictions = 0;

    Logger& logger;

public:
    LRUCache(int cap, Logger& log) : capacity(cap), logger(log) {}

    int get(int key) {
        if (cacheMap.find(key) == cacheMap.end()) {
            misses++;
            logger.log("GET FAIL", "Key=" + std::to_string(key) + " (MISS)");
            return -1;
        }

        
        cacheList.splice(cacheList.begin(), cacheList, cacheMap[key]);
        hits++;
        logger.log("GET SUCCESS", "Key=" + std::to_string(key) + " (HIT)");
        return cacheMap[key]->value;
    }

    void put(int key, int value) {
        
        if (cacheMap.find(key) != cacheMap.end()) {
            cacheMap[key]->value = value;
            cacheList.splice(cacheList.begin(), cacheList, cacheMap[key]);
            logger.log("UPDATE", "Key=" + std::to_string(key) + " Value=" + std::to_string(value));
            return;
        }

        // If at capacity, evict LRU (the back of the list)
        if (cacheList.size() == capacity) {
            int lastKey = cacheList.back().key;
            cacheMap.erase(lastKey);
            cacheList.pop_back();
            evictions++;
            logger.log("EVICT", "Key=" + std::to_string(lastKey) + " removed from cache.");
        }

         
        cacheList.emplace_front(key, value);
        cacheMap[key] = cacheList.begin();
        logger.log("PUT", "Key=" + std::to_string(key) + " Value=" + std::to_string(value));
    }

    void reset() {
        cacheList.clear();
        cacheMap.clear();
        hits = 0; misses = 0; evictions = 0;
        logger.log("RESET", "Cache cleared.");
    }

  
    const std::list<Node>& getList() const { return cacheList; }
    int getHits() const { return hits; }
    int getMisses() const { return misses; }
    int getEvictions() const { return evictions; }
    int getCapacity() const { return capacity; }
    void setCapacity(int cap) { capacity = cap; reset(); }
};

/**
 * @class UI
 * @brief Renders a modern, visual terminal interface.
 */
class UI {
public:
    static void drawHeader() {
        std::cout << CLEAR;
        std::cout << DARK_BG << WHITE << BOLD << "  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━  " << RESET << std::endl;
        std::cout << DARK_BG << CYAN << BOLD << "                     ADVANCED LRU CACHE SIMULATOR                        " << RESET << std::endl;
        std::cout << DARK_BG << WHITE << "  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━  " << RESET << std::endl;
    }

    static void drawStats(const LRUCache& cache) {
        double hitRate = (cache.getHits() + cache.getMisses() == 0) ? 0 : 
                         ((double)cache.getHits() / (cache.getHits() + cache.getMisses())) * 100;

        std::cout << "\n  " << BOLD << "SYSTEM STATS:" << RESET << std::endl;
        std::cout << "  " << GREEN << "Hits: " << cache.getHits() << RESET 
                  << "  |  " << RED << "Misses: " << cache.getMisses() << RESET 
                  << "  |  " << YELLOW << "Evictions: " << cache.getEvictions() << RESET 
                  << "  |  " << PURPLE << "Hit Rate: " << std::fixed << std::setprecision(1) << hitRate << "%" << RESET << std::endl;
    }

    static void drawCache(const LRUCache& cache) {
        std::cout << "\n  " << BOLD << "CACHE VISUALIZATION (MRU -> LRU):" << RESET << std::endl;
        std::cout << "  ";

        auto list = cache.getList();
        if (list.empty()) {
            std::cout << WHITE << "[ Empty Cache ]" << RESET << std::endl;
        } else {
            int i = 0;
            for (auto it = list.begin(); it != list.end(); ++it) {
                std::string label = "";
                std::string color = CARD_BG;
                
                if (it == list.begin()) {
                    color = GREEN; 
                    label = " (MRU)";
                } else if (std::next(it) == list.end()) {
                    color = RED;
                    label = " (LRU)";
                }

                std::cout << color << BOLD << " [" << it->key << ":" << it->value << "]" << RESET;
                
                if (std::next(it) != list.end()) {
                    std::cout << WHITE << " ⇄ " << RESET;
                }
            }
            std::cout << std::endl;
        }
        
        // Progress bar for capacity
        std::cout << "\n  Capacity Usage: [";
        int used = list.size();
        int cap = cache.getCapacity();
        for(int i=0; i<20; ++i) {
            if (i < (used * 20 / cap)) std::cout << CYAN << "■" << RESET;
            else std::cout << " ";
        }
        std::cout << "] " << used << "/" << cap << std::endl;
    }

    static void drawMenu() {
        std::cout << "\n  " << BOLD << WHITE << "CONTROLS:" << RESET << std::endl;
        std::cout << "  " << CYAN << "1. PUT (Key Value)" << RESET << "   " 
                  << CYAN << "2. GET (Key)" << RESET << "         " 
                  << CYAN << "3. RESIZE Cache" << RESET << std::endl;
        std::cout << "  " << CYAN << "4. RESET Cache" << RESET << "       " 
                  << RED << "5. EXIT" << RESET << std::endl;
        std::cout << "\n  > Selection: ";
    }

    static void showNotification(const std::string& msg, const std::string& color = YELLOW) {
        std::cout << "\n  " << color << BOLD << "» " << msg << RESET << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
};

int main() {
    Logger logger;
    LRUCache cache(4, logger); // Default capacitys 4
    int choice;

    while (true) {
        UI::drawHeader();
        UI::drawStats(cache);
        UI::drawCache(cache);
        UI::drawMenu();

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        if (choice == 5) break;

        switch (choice) {
            case 1: {
                int k, v;
                std::cout << "  Enter Key: "; std::cin >> k;
                std::cout << "  Enter Value: "; std::cin >> v;
                cache.put(k, v);
                UI::showNotification("Inserted " + std::to_string(k), GREEN);
                break;
            }
            case 2: {
                int k;
                std::cout << "  Enter Key to fetch: "; std::cin >> k;
                int val = cache.get(k);
                if (val != -1) UI::showNotification("HIT! Value: " + std::to_string(val), GREEN);
                else UI::showNotification("MISS! Key not found.", RED);
                break;
            }
            case 3: {
                int cap;
                std::cout << "  Enter New Capacity: "; std::cin >> cap;
                if (cap > 0) {
                    cache.setCapacity(cap);
                    UI::showNotification("Cache resized to " + std::to_string(cap), PURPLE);
                }
                break;
            }
            case 4:
                cache.reset();
                UI::showNotification("Cache Reset Successful", YELLOW);
                break;
            default:
                UI::showNotification("Invalid Choice", RED);
        }
    }

    std::cout << "\n  Exiting Simulator. Check cache_log.txt for history.\n" << std::endl;
    return 0;
}
