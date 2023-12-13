#include "replacementAlgo.h"
#include <algorithm>
#include <iostream>

// random
bool RandomPageReplacement::accessPage(int i, bool &hit, int &evictedPage) {
    if((*memoryMap).count(i)) {
        hit = true;
        return true;
    }
    
    hit = false;
    // since there isnt a physical page we now have to fetch from free list
    if ((*freePageList).size() == 0) {
        // no free pages
        //std::cout<<"No free pages!"<<std::endl;
        return false;
    }

    int freePage = (*freePageList).front();
    (*freePageList).pop_front();

    if (validVirtualPages.size() < 4) {
        // no replacement needed
        (*memoryMap)[i] = freePage;
        validVirtualPages.push_back(i);
    }  else {
        // evict
       int idx = uniform_distrb(gen);
       int virtPage = validVirtualPages[idx];
       evictedPage = (*memoryMap)[virtPage];
       (*freePageList).push_back(evictedPage);
       (*memoryMap).erase(virtPage);
       std::remove(validVirtualPages.begin(), validVirtualPages.end(), virtPage);
    
        // add new page
       (*memoryMap)[i] = freePage;
       validVirtualPages.push_back(i);
    }

    return true;
}

// FIFO
bool FIFOPageReplacement::accessPage(int i, bool &hit, int &evictedPage) {
    if((*memoryMap).count(i)) {
        hit = true;
        return true;
    }
    
    hit = false;
    // since there isnt a physical page we now have to fetch from free list
    if ((*freePageList).size() == 0) {
        // no free pages
        //std::cout<<"No free pages!"<<std::endl;
        return false;
    }

    int freePage = (*freePageList).front();
    (*freePageList).pop_front();

    if (validVirtualPages.size() < 4) {
        // no replacement needed
        (*memoryMap)[i] = freePage;
        validVirtualPages.push_back(i);
    }  else {
        // evict in fifo order
       int virtPage = validVirtualPages.front();
       evictedPage = (*memoryMap)[virtPage];
       (*freePageList).push_back(evictedPage);
       (*memoryMap).erase(virtPage);
       validVirtualPages.pop_front();
    
        // add new page
       (*memoryMap)[i] = freePage;
       validVirtualPages.push_back(i);
    }

    return true;
}

// LRU
bool LRUPageReplacement::accessPage(int i, bool &hit, int &evictedPage) {
    if((*memoryMap).count(i)) {
        hit = true;
        // move to bottom:
        for (auto it = validVirtualPages.begin(); it != validVirtualPages.end(); ++it) {
            if (*it == i) {
                validVirtualPages.erase(it);
                validVirtualPages.push_back(i);
                break;
            }
        }

        return true;
    }
    
    hit = false;
    // since there isnt a physical page we now have to fetch from free list
    if ((*freePageList).size() == 0) {
        // no free pages
        //std::cout<<"No free pages!"<<std::endl;
        return false;
    }

    int freePage = (*freePageList).front();
    (*freePageList).pop_front();

    if (validVirtualPages.size() < 4) {
        // no replacement needed
        (*memoryMap)[i] = freePage;
        validVirtualPages.push_back(i);
    }  else {
        // evict the Least recently used page
       int virtPage = validVirtualPages.front();
       evictedPage = (*memoryMap)[virtPage];
       (*freePageList).push_back(evictedPage);
       (*memoryMap).erase(virtPage);
       validVirtualPages.pop_front();
    
        // add new page
       (*memoryMap)[i] = freePage;
       validVirtualPages.push_back(i);
    }

    return true;
}

// LFU
bool LFUPageReplacement::accessPage(int i, bool &hit, int &evictedPage) {
    // min heap comparator:
    auto cmp = [] (std::tuple<int, int, int> const & lhs, std::tuple<int, int, int> const & rhs) {
        auto [page_l, freq_l, time_l] = lhs;
        auto [page_r, freq_r, time_r] = rhs;

        // use LRU if frequency is same
        if (freq_l == freq_r)
            return time_l > time_r;

        return freq_l > freq_r;
    };

    accessTime++;

    if((*memoryMap).count(i)) {
        hit = true;
        // move to bottom:
        for (auto it = validVirtualPages.begin(); it != validVirtualPages.end(); ++it) {
            auto [page, freq, _time] = *it;
            if (page == i) {
                validVirtualPages.erase(it);
                // heapify after removing an element from array
                std::make_heap(validVirtualPages.begin(), validVirtualPages.end(), cmp);
                validVirtualPages.push_back({i, freq + 1, accessTime});
                std::push_heap(validVirtualPages.begin(), validVirtualPages.end(), cmp);
                break;
            }
        }
        return true;
    }
    
    hit = false;
    // since there isnt a physical page we now have to fetch from free list
    if ((*freePageList).size() == 0) {
        // no free pages
        //std::cout<<"No free pages!"<<std::endl;
        return false;
    }

    int freePage = (*freePageList).front();
    (*freePageList).pop_front();

    if (validVirtualPages.size() < 4) {
        // no replacement needed
        (*memoryMap)[i] = freePage;
        validVirtualPages.push_back({i, 0, accessTime});
        std::push_heap(validVirtualPages.begin(), validVirtualPages.end(), cmp);
    }  else {
        // evict the Least frequently used page
        std::pop_heap(validVirtualPages.begin(), validVirtualPages.end(), cmp);
        auto [virtPage, freq, _time] = validVirtualPages.back();
        evictedPage = (*memoryMap)[virtPage];
        (*freePageList).push_back(evictedPage);
        (*memoryMap).erase(virtPage);
        validVirtualPages.pop_back();
    
        // add new page
        (*memoryMap)[i] = freePage;
        validVirtualPages.push_back({i, 0, accessTime});
        std::push_heap(validVirtualPages.begin(), validVirtualPages.end(), cmp);
    }

    return true;
}

// MFU
bool MFUPageReplacement::accessPage(int i, bool &hit, int &evictedPage) {
    // max heap comparator:
    auto cmp = [] (std::tuple<int, int, int> const & lhs, std::tuple<int, int, int> const & rhs) {
        auto [page_l, freq_l, time_l] = lhs;
        auto [page_r, freq_r, time_r] = rhs;

        // use MRU if frequency is same
        if (freq_l == freq_r)
            return time_l < time_r;

        return freq_l < freq_r;
    };

    accessTime++;

    if((*memoryMap).count(i)) {
        hit = true;
        // move to bottom:
        for (auto it = validVirtualPages.begin(); it != validVirtualPages.end(); ++it) {
            auto [page, freq, _time] = *it;
            if (page == i) {
                validVirtualPages.erase(it);
                // heapify after removing an element from array
                std::make_heap(validVirtualPages.begin(), validVirtualPages.end(), cmp);
                validVirtualPages.push_back({i, freq + 1, accessTime});
                std::push_heap(validVirtualPages.begin(), validVirtualPages.end(), cmp);
                break;
            }
        }
        return true;
    }
    
    hit = false;
    // since there isnt a physical page we now have to fetch from free list
    if ((*freePageList).size() == 0) {
        // no free pages
        //std::cout<<"No free pages!"<<std::endl;
        return false;
    }

    int freePage = (*freePageList).front();
    (*freePageList).pop_front();

    if (validVirtualPages.size() < 4) {
        // no replacement needed
        (*memoryMap)[i] = freePage;
        validVirtualPages.push_back({i, 0, accessTime});
        std::push_heap(validVirtualPages.begin(), validVirtualPages.end(), cmp);
    }  else {
        // evict the most frequently used page
        std::pop_heap(validVirtualPages.begin(), validVirtualPages.end(), cmp);
        auto [virtPage, freq, _time] = validVirtualPages.back();
        evictedPage = (*memoryMap)[virtPage];
        (*freePageList).push_back(evictedPage);
        (*memoryMap).erase(virtPage);
        validVirtualPages.pop_back();
    
        // add new page
        (*memoryMap)[i] = freePage;
        validVirtualPages.push_back({i, 0, accessTime});
        std::push_heap(validVirtualPages.begin(), validVirtualPages.end(), cmp);
    }

    return true;
}