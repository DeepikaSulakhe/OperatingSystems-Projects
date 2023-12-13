package com.scu.secondassignment;

import java.util.concurrent.atomic.AtomicBoolean;

public class Consumer extends Thread{
	private AtomicBoolean shouldContinue;
    private CircularBuffer circularBuffer;
    private ThreadSafePrint threadSafePrint;

    public Consumer(AtomicBoolean shouldContinue, CircularBuffer circularBuffer, ThreadSafePrint threadSafePrint) {
        this.shouldContinue = shouldContinue;
        this.circularBuffer = circularBuffer;
        this.threadSafePrint = threadSafePrint;
    }


    public void run() {
        threadSafePrint.safePrint("Consumer Thread: " + Thread.currentThread().threadId() + " started");
        while(shouldContinue.get()) {
            int data = circularBuffer.popFront();
            if (shouldContinue.get()) {
                threadSafePrint.safePrint("Consumer Thread: " + Thread.currentThread().threadId() + " consumed data: " + data);
            }
        }
        threadSafePrint.safePrint("Consumer Thread: " + Thread.currentThread().threadId() + " exiting !");
    }
}
