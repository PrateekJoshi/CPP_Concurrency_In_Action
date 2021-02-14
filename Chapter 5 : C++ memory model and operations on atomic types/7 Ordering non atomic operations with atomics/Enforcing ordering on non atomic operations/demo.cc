/*
 * demo.cc
 *
 *  Created on: 14-Feb-2021
 *      Author: prateek
 *      Pg 145
 The fences still provide an enforced ordering of the store to x B and the store to y c
and the load from y d and the load from x e, and there’s still a happens-before rela-
tionship between the store to x and the load from x, so the assert f still won’t fire.
The store to c and load from d y still have to be atomic; otherwise, there would be a
data race on y , but the fences enforce an ordering on the operations on x, once the
reading thread has seen the stored value of y . This enforced ordering means that there’s
no data race on x, even though it’s modified by one thread and read by another.


The lock() operation is a loop on flag.test_and_set() using std::memory_
order_acquire ordering, and the unlock() is a call to flag.clear() with std::
memory_order_release ordering. When the first thread calls lock(), the flag is ini-
tially clear, so the first call to test_and_set() will set the flag and return false, indi-
cating that this thread now has the lock, and terminating the loop. The thread is then
free to modify any data protected by the mutex. Any other thread that calls lock() at
this time will find the flag already set and will be blocked in the test_and_set() loop.

 */
#include <atomic>
#include <thread>
#include <assert.h>

bool x=false;
std::atomic<bool> y;
std::atomic<int> z;

void write_x_then_y()
{
    x=true;
    std::atomic_thread_fence(std::memory_order_release);
    y.store(true,std::memory_order_relaxed);
}

void read_y_then_x()
{
    while(!y.load(std::memory_order_relaxed));
    std::atomic_thread_fence(std::memory_order_acquire);
    if(x)
        ++z;
}

int main()
{
    x=false;
    y=false;
    z=0;
    std::thread a(write_x_then_y);
    std::thread b(read_y_then_x);
    a.join();
    b.join();
    assert(z.load()!=0);
}



