#include "utils.h"

class Simulation {
public:
    Simulation(PageReplacementAlgo pageReplacementAlgo);
    ~Simulation() = default;
    double runSimulation();
private:
    PageReplacementAlgo pageReplacementAlgo;
    double simulationDuration_sec = 60.0;
    int mainMemoryPages = 100;
    int numProcesses = 150;
 
    // active runnable queue
    std::list<Process> jobQueue;
    
    // free memory list
    std::list<int> freePageList;
};