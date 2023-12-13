#include <map>
#include <list>
#include <vector>
#include <random>

class PageReplacement {
public:
    PageReplacement() = default;
    virtual ~PageReplacement() = default;

    virtual bool accessPage(int i, bool &hit, int &evictedPage) = 0;
};

class RandomPageReplacement: public PageReplacement {
public:
    RandomPageReplacement(std::map<int,int> &memoryMap, std::list<int> *freePageList){
        this->freePageList = freePageList;
        this->memoryMap = &memoryMap;
        std::random_device rd;
        gen = std::mt19937(rd());
        uniform_distrb = std::uniform_int_distribution<>(0, 3);
    }

    ~RandomPageReplacement() = default;
    bool accessPage(int i, bool &hit, int &evictedPage) override;

private:
    std::uniform_int_distribution<int> uniform_distrb;
    std::mt19937 gen;
    std::map<int,int> *memoryMap;
    std::list<int> *freePageList;
    std::vector<int> validVirtualPages;
};

class FIFOPageReplacement: public PageReplacement {
public:
    FIFOPageReplacement(std::map<int,int> &memoryMap, std::list<int> *freePageList){
        this->freePageList = freePageList;
        this->memoryMap = &memoryMap;
    }

    ~FIFOPageReplacement() = default;
    bool accessPage(int i, bool &hit, int &evictedPage) override;

private:
    std::map<int,int> *memoryMap;
    std::list<int> *freePageList;
    
    // fifo tracker:
    std::list<int> validVirtualPages;
};

class LRUPageReplacement: public PageReplacement {
public:
    LRUPageReplacement(std::map<int,int> &memoryMap, std::list<int> *freePageList){
        this->freePageList = freePageList;
        this->memoryMap = &memoryMap;
    }

    ~LRUPageReplacement() = default;
    bool accessPage(int i, bool &hit, int &evictedPage) override;

private:
    std::map<int,int> *memoryMap;
    std::list<int> *freePageList;
    
    // LRU linkedlist tracker:
    std::list<int> validVirtualPages;
};

class LFUPageReplacement: public PageReplacement {
public:
    LFUPageReplacement(std::map<int,int> &memoryMap, std::list<int> *freePageList){
        this->freePageList = freePageList;
        this->memoryMap = &memoryMap;
    }

    ~LFUPageReplacement() = default;
    bool accessPage(int i, bool &hit, int &evictedPage) override;

private:
    std::map<int,int> *memoryMap;
    std::list<int> *freePageList;
    
    // lfu tracker:
    // {page, count, accesTime};
    int accessTime = 0;
    std::vector<std::tuple<int, int, int>> validVirtualPages;
};

// MFU
class MFUPageReplacement: public PageReplacement {
public:
    MFUPageReplacement(std::map<int,int> &memoryMap, std::list<int> *freePageList){
        this->freePageList = freePageList;
        this->memoryMap = &memoryMap;
    }

    ~MFUPageReplacement() = default;
    bool accessPage(int i, bool &hit, int &evictedPage) override;

private:
    std::map<int,int> *memoryMap;
    std::list<int> *freePageList;
    
    // mfu tracker:
    // {page, count, accesTime};
    int accessTime = 0;
    std::vector<std::tuple<int, int, int>> validVirtualPages;
};