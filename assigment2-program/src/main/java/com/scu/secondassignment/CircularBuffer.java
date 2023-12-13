package com.scu.secondassignment;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class CircularBuffer {
    // Size of the buffer
	private int capacity;

    // Current size
    private int currSize;

    // Data will dequeued from front index
    private int front = 0;

    // Data will be pushed in from the back index
    private int back = -1;
    private int[] array;
    private ReentrantLock lockObj = new ReentrantLock();
    //bufferIsNotFull will be signaled by consumer and waited by producer when buffer is full
    private Condition bufferIsNotFull = lockObj.newCondition();
    //bufferIsNotEmpty will be signaled by producer and waited on consumer when buffer is empty
    private Condition bufferIsNotEmpty = lockObj.newCondition();

    // Atomic boolean to control exit when all the data is consumed
    private AtomicBoolean shouldContinue;


    public CircularBuffer(int capacity, AtomicBoolean shouldContinue) {
        this.shouldContinue = shouldContinue;
        this.capacity = capacity;
        this.array = new int[capacity];
    }

    // should be called with lock held
    public boolean isEmpty() {
        return currSize == 0; 
    }

    // should be called with lock held
    public boolean isFull() {
        return currSize == capacity;
    }

    // waits for consumer to consume all the given data and then sets shouldContinue to false
    // followed by signaling bufferIsNotEmpty condition to wakes up all the blocked consumers
    public void waitTillConsumed() {
        lockObj.lock();
        try {
            while(!isEmpty()) {
                bufferIsNotFull.await(5, TimeUnit.SECONDS);
            }
            shouldContinue.set(false);
            bufferIsNotEmpty.signalAll();
        } catch(InterruptedException exception) {
          Thread.currentThread().interrupt(); // set interrupted flag  
        } finally {
            lockObj.unlock();
        }
    }
   
    //pushBack will be called by producer 
    public void pushBack(int data) {
        lockObj.lock();
        try {
            while (isFull()) {
                // wait for consumer to consume data
                bufferIsNotFull.await();
            }
            ++currSize;
            ++back;
            back %= capacity;
            array[back] = data;
            // signal a waiting consumer of new incoming data
            bufferIsNotEmpty.signal();
        } catch(InterruptedException exception) {
            Thread.currentThread().interrupt(); // set interrupted flag  
        } finally {
            lockObj.unlock();
        }
    }
    //popFront will be called by consumer threads
    public int popFront() {
        if (!shouldContinue.get()) {
            return -1;
        }
        int data = -1;
        lockObj.lock();
        try {
            while (isEmpty()) {
                // wait for producer to produce data
                bufferIsNotEmpty.await();
                if (!shouldContinue.get()) {
                    return -1;
                }
            }
            data = array[front];
            --currSize;
            if (front == back) {
                front = 0;
                back  = -1;
            } else {
                ++front;
                front %= capacity;
            }
    
            // signal producer since 1 data item has been consumed
            bufferIsNotFull.signal();
        } catch(InterruptedException exception) {
            Thread.currentThread().interrupt(); // set interrupted flag  
        } finally {
            lockObj.unlock();
        }
        return data;
    }
}
