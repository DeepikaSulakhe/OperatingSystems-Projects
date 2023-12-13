#include "simulation.h"
#include <thread>
#include <iostream>

Simulation::Simulation(PageReplacementAlgo pageReplacementAlgo) {
    this->pageReplacementAlgo = pageReplacementAlgo;

    for(int i = 0; i < mainMemoryPages; ++i) {
       freePageList.push_back(i);
    }
}

double Simulation::runSimulation() {
    Timer timer;
    RandomProcessGenerator randomGenerator;
    int pid = 1;
    double hitCountTotal = 0;
    double missCountTotal = 0;

    while(timer.elapsed() < simulationDuration_sec) {
        // check if a job can be added to ready queue:
        if (freePageList.size() >= 4 && pid <= numProcesses) {
            jobQueue.emplace_back(Process());
            randomGenerator.populateProcess(pid, freePageList, jobQueue.back());
            jobQueue.back().initialize(timer.start, pageReplacementAlgo);
            ++pid;
        }
    
        // tick all the process to check for memory access
        std::list<Process>::iterator it = jobQueue.begin();
        while(it != jobQueue.end()) {
            bool shouldContinue = it->tick(timer.start);
            // remove the job that has finished its job duration
            
            if (!shouldContinue) {
                hitCountTotal += static_cast<double>(it->hitCount);
                missCountTotal += static_cast<double>(it->missCount);
                it->destroy(timer.start);
                it = jobQueue.erase(it);
            } else {
                ++it;
            }
        }
        // tick every 10 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout<<"Simulation finished! elapsed sec: "<< timer.elapsed()<< " removing remaining jobs from the queue"<<std::endl;
    std::list<Process>::iterator it = jobQueue.begin();
    while(it != jobQueue.end()) {
        hitCountTotal += static_cast<double>(it->hitCount);
        missCountTotal += static_cast<double>(it->missCount);
        it->destroy(timer.start);
        ++it;
    }

    return hitCountTotal/missCountTotal;
}