/*
 * demo.cc
 *
 *  Created on: 10-Feb-2021
 *      Author: prateek
 *      Pg 111
std::atomic_flag is the simplest standard atomic type, which represents a Boolean
flag. Objects of this type can be in one of two states: set or clear. It’s deliberately basic
and is intended as a building block only.

Objects of type std::atomic_flag must be initialized with ATOMIC_FLAG_INIT . This ini-
tializes the flag to a clear state. There’s no choice in the matter; the flag always starts clear:
std::atomic_flag f=ATOMIC_FLAG_INIT;
This applies wherever the object is declared and whatever scope it has. It’s the only
atomic type to require such special treatment for initialization, but it’s also the only type
guaranteed to be lock-free. If the std::atomic_flag object has static storage duration,
it’s guaranteed to be statically initialized, which means that there are no initialization-
order issues; it will always be initialized by the time of the first operation on the flag.

The limited feature set makes std::atomic_flag ideally suited to use as a spin-
lock mutex. Initially the flag is clear and the mutex is unlocked. To lock the mutex,
loop on test_and_set() until the old value is false, indicating that this thread set the
value to true. Unlocking the mutex is simply a matter of clearing the flag. Such an
implementation is shown in the following listing.
 */
#include <atomic>

class spinlock_mutex
{
	std::atomic_flag flag;

	spinlock_mutex() : flag(ATOMIC_FLAG_INIT)
	{}

	void lock()
	{
		while(flag.test_and_set(std::memory_order_acquire));
	}

	void unlock()
	{
		flag.clear(std::memory_order_release);
	}
};




