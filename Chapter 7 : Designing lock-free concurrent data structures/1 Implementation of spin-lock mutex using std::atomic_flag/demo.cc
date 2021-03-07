/*
 * demo.cc
 *
 *  Created on: 01-Mar-2021
 *      Author: prateek
 *      Pg 181
 This code doesn’t call any blocking functions; lock() just keeps looping until the call
to test_and_set() returns false. This is why it gets the name spin lock—the code
“spins” around the loop. Anyway, there are no blocking calls, so any code that uses this
mutex to protect shared data is consequently nonblocking. It’s not lock-free, though. It’s
still a mutex and can still be locked by only one thread at a time.
 */
#include <atomic>

class spinlock_mutex
{
	std::atomic_flag flag;
public:
	/* Constructor */
	spinlock_mutex() : flag(ATOMIC_FLAG_INIT)
	{}

	/* Lock the mutex */
	void lock()
	{
		while(flag.test_and_set(std::memory_order_acquire));
	}

	/* Unlock mutex */
	void unlock()
	{
		flag.clear(std::memory_order_release);
	}
};


