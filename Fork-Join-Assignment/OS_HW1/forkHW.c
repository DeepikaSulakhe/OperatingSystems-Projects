#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEBUG 0
void spawnProcesses(int numProcesses) {
    pid_t leftChild;
    pid_t rightChild;
    bool leftChildSpawned = false;
    bool rightChildSpawaned = false;
    int i = 0;
    int status;
    while (i <= numProcesses/2) {
        leftChildSpawned = false;
        rightChildSpawaned = false;
        leftChild = fork();
        if (leftChild < 0) {
            printf("Fork call failed!\n");
            exit(1);
        } else if (leftChild == 0) {
            i = 2 * i + 1;
            printf("Left  Child process created pid: %d, by parent pid: %d, index: %d\n", getpid(), getppid(), i);
        } else {
            if ( 2 * i + 2 > numProcesses) {
                leftChildSpawned = true;
                break;
            }
            rightChild = fork();
            if (rightChild < 0) {
                printf("Fork call failed!\n");
                exit(1);
            } else if (rightChild == 0) {
                i = 2 * i + 2;
                printf("Right Child process created pid: %d, by parent pid: %d, index: %d\n", getpid(),  getppid(), i);
            } else {
                leftChildSpawned = true;
                rightChildSpawaned = true;
                break;
            }
        }
    }

    if (leftChildSpawned) {
        waitpid(leftChild, &status, 0);
#if DEBUG
        printf("Left  Child process with pid: %d exited with status: %d wait called by pid: %d\n", leftChild, WEXITSTATUS(status), getpid());
#endif
    }
    if (rightChildSpawaned) {
        waitpid(rightChild, &status, 0);
#if DEBUG
        printf("Right Child process with pid: %d exited with status: %d wait called by pid: %d\n", rightChild, WEXITSTATUS(status), getpid());
#endif
    }
}

int main(void)
{
    printf("Original root parentProcess pid: %d\n", getpid());
    int numProcessesToSpawn = 9;
    spawnProcesses(numProcessesToSpawn);
    return 0;
}