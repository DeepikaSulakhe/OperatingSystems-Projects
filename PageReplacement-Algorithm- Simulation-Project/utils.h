#include "replacementAlgo.h"
#include <random>
#include <chrono>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>

enum class PageReplacementAlgo {
    RANDOM = 0,
    FIFO = 1,
    LRU = 2,
    LFU = 3,
    MFU = 4
};

const std::string  algoName[5] = {"Random", "FIFO", "LRU", "LFU", "MFU"};
class RandomGenerator {
public:
    RandomGenerator(int a, int b);
    ~RandomGenerator() = default;
    int getNextPageReference(int prevPage);

private:
    int minI, maxI;
    std::uniform_int_distribution<int> uniform_int_distrb;
    std::mt19937 gen;

    int deltaI[3] = {-1, 0, 1};
    std::uniform_int_distribution<int> uniform_deltaI_distrb;
    std::uniform_int_distribution<int> uniform_j_distrb;
};

struct Process;
class RandomProcessGenerator {
public:
    RandomProcessGenerator();
    ~RandomProcessGenerator() = default;
    void populateProcess(int pid, std::list<int> &freePageList, Process &outProcess);
private:
    std::uniform_int_distribution<int> uniform_pageSize_distrb;
    std::uniform_int_distribution<int> uniform_serviceTime_distrb;
    std::mt19937 gen;

    int pageSizes[4] = {5, 11, 17, 31};
};

struct Timer
{
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) {}
    double elapsed() 
    { 
        auto curr = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration<double, std::milli>(curr - start).count() / 1000.0;
        return elapsedTime;
    }
    std::chrono::high_resolution_clock::time_point start;
};

struct Process {
    Process() = default;
    ~Process() {
        if(replacementAlgo != nullptr) {
            delete replacementAlgo;
            replacementAlgo = nullptr;
        }
    }

    void initialize(std::chrono::high_resolution_clock::time_point start, PageReplacementAlgo pageReplacementAlgo);
    bool tick(std::chrono::high_resolution_clock::time_point start);
    void destroy(std::chrono::high_resolution_clock::time_point start);
    int processName;
    int totalSizeInPages;
    int serviceDuration_sec;
    std::chrono::high_resolution_clock::time_point arrivalTime;
    std::chrono::high_resolution_clock::time_point lastAccessTime;
    int lastAccessPage = -1;
    std::map<int, int> memoryMap;
    std::unordered_set<int> uniqueAccessSet;
    std::list<int> *freePageList;
    RandomGenerator pageAccessor = RandomGenerator(0, 10);

    // replacement algo:
    PageReplacement *replacementAlgo = nullptr;
    // statistics:
    int pageAccessCount = 0;
    int hitCount = 0;
    int missCount = 0;
};