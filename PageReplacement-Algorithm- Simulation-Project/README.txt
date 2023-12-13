## For running: (shown in Demo video)
Step:1 --> make clean && make
Step :2 -->
# By default all output will be to stdout: 
Command: ./simulator <number> 
where <number> is algo number
0 for RANDOM
1 for FIFO
2 for LRU
3 for LFU 
4 for MFU

# If you want to save it to log file run as follows:

>> This command does not show anything in console, It prints whole output in log file (number 2 is for LRU it saves in LRULog.txt)
Command: ./simulator 2 > LRULog.txt 
and all the output will be saved to LRULog.txt

>>This command shows the statistics for each simulator iteration(1,2,34,5) and final average and saves the whole output in given log file.
Command: ./simulator 0 | tee  RANDOMLog.txt | grep -i -e "Stats for iteration" -e "Average hit miss ratio for algo"
(OR) you can run all the 5 algorithms at a time in different terminal bash by updating the above command <number> and log file name.

# After giving this command it will run for 1 min per iteration so total 5 mins for 5 iterations  as given in homework problem statement. You can view the output log file for the detailed output.
>> While submitting I will provide the .zip file with all the output log files you can re-run in your device with the above given commands it will replace the whole new output file.

###Average hit miss ratio for all the page reference algorithms:
>>Average hit miss ratio for algo: Random averaged over 5 iterations is: 0.568513.
>>Average hit miss ratio for algo: FIFO averaged over 5 iterations is: 0.502295.
>> Average hit miss ratio for algo: LRU averaged over 5 iterations is: 0.508089.
>>Average hit miss ratio for algo: LFU averaged over 5 iterations is: 0.484547.
>>Average hit miss ratio for algo: MFU averaged over 5 iterations is: 0.495402.


## About the code structure:
1. utils.cpp : RandomGeneration algorithm for Locality of reference implementation.
> It contains all the RandomGeneration logic and also have process structure and process lifetime logic.
2.simulation.cpp: main simulation logic to run simulation for 60 sec , generate 150 process, schedule each process into jobs and generate access every 100 milliseconf for each process.
3. replacementAlgo.cpp : contains logic for all the Page replacement algorithms asked to implement in the problem statement.
4.main.cpp : Taking inputs to select which algorithm and run the simulation for 5 iterations using selected algorithm and printing the final statistics.

#Please verify output pdf for output screen shot and for best and worst algorithms.
