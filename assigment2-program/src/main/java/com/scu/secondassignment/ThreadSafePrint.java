package com.scu.secondassignment;

import java.util.concurrent.locks.ReentrantLock;

public class ThreadSafePrint {
	private ReentrantLock lockObj = new ReentrantLock();
    public ThreadSafePrint() {}
    //Required for safe printing to console since print is used by multiple threads
    public void safePrint(String s) {
        lockObj.lock();
        System.out.println(s);
        lockObj.unlock();
    }
}
