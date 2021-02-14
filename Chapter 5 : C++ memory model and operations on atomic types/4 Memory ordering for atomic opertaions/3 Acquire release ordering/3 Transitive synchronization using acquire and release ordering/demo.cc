/*
 * demo.cc
 *
 *  Created on: 14-Feb-2021
 *      Author: prateek
 *      Pg 136
 In order to think about transitive ordering, you need at least three threads. The first
thread modifies some shared variables and does a store-release to one of them. A sec-
ond thread then reads the variable subject to the store-release with a load-acquire and
performs a store-release on a second shared variable. Finally, a third thread does a
load-acquire on that second shared variable. Provided that the load-acquire opera-
tions see the values written by the store-release operations to ensure the synchronizes-
with relationships, this third thread can read the values of the other variables stored
by the first thread, even if the intermediate thread didn’t touch any of them. This sce-
nario is shown in the next listing.

Even though thread_2 only touches the variables sync1 c and sync2 d, this is
enough for synchronization between thread_1 and thread_3 to ensure that the
asserts don’t fire. First off, the stores to data from thread_1 happens-before the store
to sync1 B, because they’re sequenced-before it in the same thread. Because the load
from sync1 B is in a while loop, it will eventually see the value stored from thread_1
and thus form the second half of the release-acquire pair. Therefore, the store to
sync1 happens-before the final load from sync1 in the while loop. This load is
sequenced-before (and thus happens-before) the store to sync2 d, which forms a
release-acquire pair with the final load from the while loop in thread_3 e. The store
to sync2 d thus happens-before the load e, which happens-before the loads from
data. Because of the transitive nature of happens-before, you can chain it all together:
the stores to data happen-before the store to sync1 B, which happens-before the
load from sync1 c, which happens-before the store to sync2 d, which happens-
before the load from sync2 e, which happens-before the loads from data. Thus the
stores to data in thread_1 happen-before the loads from data in thread_3, and
the asserts can’t fire.
 */
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<int> data[5];
std::atomic<bool> sync1(false), sync2(false);

void thread_1()
{
	data[0].store(42, std::memory_order_relaxed);
	data[1].store(97, std::memory_order_relaxed);
	data[2].store(17, std::memory_order_relaxed);
	data[3].store(-141, std::memory_order_relaxed);
	data[4].store(2003, std::memory_order_relaxed);

	sync1.store(true, std::memory_order_release);		// Set sync1
}

void thread_2()
{
	while( !sync1.load(std::memory_order_acquire));		// Loop until sync 1 is set
	sync2.store(true, std::memory_order_release);		// Set sync2
}

void thread_3()
{
	while( !sync2.load(std::memory_order_acquire) );	// Loop until sync2 is set

	/* None of the assertions will fail */
	assert(data[0].load(std::memory_order_relaxed) == 42);
	assert(data[1].load(std::memory_order_relaxed) == 97);
	assert(data[2].load(std::memory_order_relaxed) == 17);
	assert(data[3].load(std::memory_order_relaxed) == -141);
	assert(data[4].load(std::memory_order_relaxed) == 2003);
}





























