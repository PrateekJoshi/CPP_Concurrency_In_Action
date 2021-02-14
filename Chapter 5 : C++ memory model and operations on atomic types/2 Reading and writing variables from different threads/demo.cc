/*
 * demo.cc
 *
 *  Created on: 13-Feb-2021
 *      Author: prateek
 *      Pg 119
Suppose you have two threads, one of which is populating a data structure to be read
by the second. In order to avoid a problematic race condition, the first thread sets a
flag to indicate that the data is ready, and the second thread doesn’t read the data
until the flag is set. The following listing shows such a scenario.

The required enforced ordering comes from the operations on the std::
atomic<bool> variable data_ready; they provide the necessary ordering by virtue of
the memory model relations happens-before and synchronizes-with. The write of the data d
happens-before the write to the data_ready flag e, and the read of the flag B hap-
pens-before the read of the data c. When the value read from data_ready B is true,
the write synchronizes-with that read, creating a happens-before relationship. Because
happens-before is transitive, the write to the data d happens-before the write to the
flag e, which happens-before the read of the true value from the flag B, which
happens-before the read of the data c, and you have an enforced ordering: the write
of the data happens-before the read of the data and everything is OK. Figure 5.2 shows
the important happens-before relationships in the two threads. I’ve added a couple of
iterations of the while loop from the reader thread.
 */
#include <vector>
#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>

std::vector<int> data;
std::atomic_bool data_ready(false);

void reader_thread()
{
	while( !data_ready.load() )
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	std::cout<<"The answer : "<<data[0]<<std::endl;
}

void writer_thread()
{
	data.push_back(42);
	data_ready=true;
}































