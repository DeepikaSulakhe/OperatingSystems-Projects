package com.scu.secondassignment;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Hello world!
 *
 */
public class Main 
{
    public static void main( String[] args )
    {
    	int numConsumerThreads = 1;
        int bufferCapacity = 5;
        if (args.length > 0) {
            try {
                numConsumerThreads = Integer.parseInt(args[0]);
                System.out.println("Number of consumer threads to be spawned: " + numConsumerThreads);
            } catch (NumberFormatException e) {
                System.out.println("Invalid integer input");
                return;
            }
        }
        System.out.println("CircularBuffer capacity is set to: " + bufferCapacity);

        // create shared objects
        AtomicBoolean shouldContinue = new AtomicBoolean(true);
        CircularBuffer circularBuffer = new CircularBuffer(bufferCapacity, shouldContinue);
        ThreadSafePrint threadSafePrint = new ThreadSafePrint();

        // create producer thread
        Producer producer = new Producer(circularBuffer, threadSafePrint);

        // create consumer threads
        Consumer [] consumerList = new Consumer[numConsumerThreads];
        for (int i = 0; i < numConsumerThreads; ++i) {
            consumerList[i] = new Consumer(shouldContinue, circularBuffer, threadSafePrint);
        }

        // start the consumer threads
        for (int i = 0; i < numConsumerThreads; ++i) {
            consumerList[i].start();
        }

        // start the producer thread
        producer.start();

        try {
            producer.join();

            // producer has finished producing. wait until consumers finish consuming before calling waitTillConsumed
            circularBuffer.waitTillConsumed();

            // cleanly join the consumer threads as they should have finished executing
            for (int i = 0; i < numConsumerThreads; ++i) {
                consumerList[i].join();
            }
            System.out.println("All Producer and Consumer threads joined successfully");
        } catch (InterruptedException e) {
           System.err.println("Error joining Threads");
           e.printStackTrace();
        }


    }
}
