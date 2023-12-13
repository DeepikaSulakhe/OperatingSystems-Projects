package com.scu.secondassignment;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Producer extends Thread {
	private CircularBuffer circularBuffer;
    private ThreadSafePrint threadSafePrint;

    public Producer(CircularBuffer circularBuffer, ThreadSafePrint threadSafePrint) {
        this.circularBuffer = circularBuffer;
        this.threadSafePrint = threadSafePrint;
    }

    public void run() {
        threadSafePrint.safePrint("Producer Thread: " + Thread.currentThread().threadId() + " started");
        threadSafePrint.safePrint("Please enter numbers separated by space: ");
        //BufferedReader, input will be read or accepted in a single line separated by one or two spaces
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        String[] arr;
        try {
            arr = reader.readLine().split("\\s+");
        } catch (IOException e) {
             System.out.println("Unable to read input");
            return;
        }
        int[] intArr = new int[arr.length];

        for(int i=0; i < arr.length; i++) {
            try {
                intArr[i]=Integer.parseInt(arr[i]);
            } catch (NumberFormatException e) {
                System.out.println("Invalid integer input");
                return;
            }
        }

        threadSafePrint.safePrint("Producer Thread: " + Thread.currentThread().threadId() + " Pushing " + intArr.length +
                                   " parsed integers into circular buffer");
        
        // push the data for consumers to consume
        for (int i = 0; i < intArr.length; ++i) {
            circularBuffer.pushBack(intArr[i]);
        }

        threadSafePrint.safePrint("Producer Thread: " + Thread.currentThread().threadId() + " exiting !");
    }
}
