#include "utils.h"
#include <iostream>
#include <vector>
#include <unordered_set>

RandomGenerator::RandomGenerator(int a, int b): minI(a), maxI(b) {
    std::random_device rd;
    gen = std::mt19937(rd());
    uniform_int_distrb = std::uniform_int_distribution<>(a, b);
    uniform_deltaI_distrb = std::uniform_int_distribution<>(0, 2);
    int sum = maxI - minI + 1;
    uniform_j_distrb = std::uniform_int_distribution<>(0, sum - 4);
}

int RandomGenerator::getNextPageReference(int i) {

    int r = uniform_int_distrb(gen);
    if (r < 7) {
        int idx = uniform_deltaI_distrb(gen);
        int sum = maxI - minI + 1;
        //std::cout<<" prev i "<< i << " 70 percent probabily: ";
        return (sum + (i + deltaI[idx]) % (sum)) % sum;
    } else {
        // make a list of indexes excluding i, i + 1 and i - 1
        int sum = maxI - minI + 1;
        std::vector<int> arr;
        std::unordered_set<int> excludeSet;
        excludeSet.insert(i);
        excludeSet.insert((sum + (i + 1) % (sum)) % sum);
        excludeSet.insert((sum + (i - 1) % (sum)) % sum);

        for(int j = minI; j <= maxI; ++j) {
            if (excludeSet.count(j) == 0) {
                arr.push_back(j);
            }
        }
        
        //std::cout<<" prev i "<< i << " 30 percent probabily: ";
        int idx = uniform_j_distrb(gen);
        return arr[idx];
    }
}

RandomProcessGenerator::RandomProcessGenerator() {
    std::random_device rd;
    gen = std::mt19937(rd());
    uniform_pageSize_distrb = std::uniform_int_distribution<>(0, 3);
    uniform_serviceTime_distrb = std::uniform_int_distribution<>(1, 10);
}

void RandomProcessGenerator::populateProcess(int pid, std::list<int> &freePageList, Process &outProcess) {
    outProcess.processName = pid;

    outProcess.freePageList = &freePageList;
    int idx = uniform_pageSize_distrb(gen);
    outProcess.totalSizeInPages  = pageSizes[idx];
    outProcess.serviceDuration_sec =  uniform_serviceTime_distrb(gen);
}

// Process methods
void Process::initialize(std::chrono::high_resolution_clock::time_point start, PageReplacementAlgo pageReplacementAlgo) {
    arrivalTime = std::chrono::high_resolution_clock::now();
    lastAccessTime = arrivalTime;
    lastAccessPage = 0;

    switch (pageReplacementAlgo)
    {
        case PageReplacementAlgo::RANDOM:
            replacementAlgo = new RandomPageReplacement(memoryMap, freePageList);
            break; 
        case PageReplacementAlgo::FIFO:
            replacementAlgo = new FIFOPageReplacement(memoryMap, freePageList);
            break;
        case PageReplacementAlgo::LRU:
            replacementAlgo = new LRUPageReplacement(memoryMap, freePageList);
            break;
        case PageReplacementAlgo::LFU:
            replacementAlgo = new LFUPageReplacement(memoryMap, freePageList);
            break;
        case PageReplacementAlgo::MFU:
            replacementAlgo = new MFUPageReplacement(memoryMap, freePageList);
            break;     
        default:
            break;
    }

    // access page 0 here
    bool hit = false;
    int evictedPage = -1;
    if (replacementAlgo->accessPage(0, hit, evictedPage)) {
        pageAccessCount++;
        uniqueAccessSet.insert(0);
    }

    // update hit miss counts:
    if (hit) {
        hitCount++;
    } else {
        missCount++;
    }

    // print record:
    auto elapsedSinceStart = std::chrono::duration<double, std::milli>(arrivalTime - start).count() / 1000.0;
    std::cout<<"ProcessName: "<< processName;
    std::cout<<", addded to jobQueue. timestamp:" << elapsedSinceStart;
    std::cout<<", size in Pages: "<<totalSizeInPages;
    std::cout<<", pagesAcessed start: "<<pageAccessCount;
    std::cout<<", uniquePagesAccessed: "<<uniqueAccessSet.size();
    std::cout<<", service duration: "<< serviceDuration_sec << std::endl;
    for(auto &[i, j] : memoryMap) {
        std::cout<<"M["<<i<<"] = "<<j<<", ";
    }
    std::cout<<std::endl;
}

void Process::destroy(std::chrono::high_resolution_clock::time_point start) {
    auto curr = std::chrono::high_resolution_clock::now();
    // print record:
    auto elapsedSinceStart = std::chrono::duration<double, std::milli>(curr - start).count() / 1000.0;
    std::cout<<"ProcessName: "<< processName;
    std::cout<<", finished and removed from jobQueue. timestamp:" << elapsedSinceStart;
    std::cout<<", size in Pages: "<<totalSizeInPages;
    std::cout<<", pagesAcessed exit: "<<pageAccessCount;
    std::cout<<", uniquePagesAccessed: "<<uniqueAccessSet.size();
    std::cout<<", hit count: "<<hitCount;
    std::cout<<", miss count: "<<missCount;
    std::cout<<", service duration: "<< serviceDuration_sec <<std::endl;
    for(auto &[i, j] : memoryMap) {
        std::cout<<"M["<<i<<"] = "<<j<<", ";
    }
    std::cout<<std::endl;

    // release all pages
    for(auto &[i, j] : memoryMap) {
        freePageList->push_back(j);
    }

    memoryMap.clear();
}

bool Process::tick(std::chrono::high_resolution_clock::time_point start) {
    auto curr = std::chrono::high_resolution_clock::now();

    auto elapsedTime = std::chrono::duration<double, std::milli>(curr - arrivalTime).count() / 1000.0;
    if (elapsedTime > static_cast<double>(serviceDuration_sec)) {
        return false;
    }

    auto elapsedMilliseconds = std::chrono::duration<double, std::milli>(curr - lastAccessTime).count();
  
    if (elapsedMilliseconds < 100) {
        return true;
    }

    lastAccessTime = curr;

    int i = pageAccessor.getNextPageReference(lastAccessPage);
    lastAccessPage = i;

    bool hit = false;
    int evictedPage = -1;
    bool pageAccessed = replacementAlgo->accessPage(i, hit, evictedPage);
    if (pageAccessed) {
        pageAccessCount++;
        uniqueAccessSet.insert(i);
    }

    // update hit miss counts:
    if (hit) {
        hitCount++;
    } else {
        missCount++;
    }

    // print record:
    auto elapsedSinceStart = std::chrono::duration<double, std::milli>(curr - start).count() / 1000.0;
    std::cout<<"timeStamp: "<< elapsedSinceStart;
    std::cout<<", processName: "<< processName;
    if (pageAccessed) {
        std::cout<<", page referenced: "<<i;
        std::cout<<", physical mem paged in: "<< memoryMap.at(i);
        if(evictedPage >= 0) {
            std::cout<<", physical mem evicted: "<<evictedPage;
        }
    } else {
        std::cout<< " tried refrencing "<<i<< " but no physical page available!";
    }
    std::cout<<std::endl;
    return true; 
}