/*
 * demo.cc
 *
 *  Created on: 13-Feb-2021
 *      Author: prateek
 *      Pg 129
You have three shared global atomic variables B and five threads.
Each thread loops 10 times, reading the values of the three
atomic variables using memory_order_relaxed and storing them in an array. Three of
the threads each update one of the atomic variables each time through the loop e,
while the other two threads just read. Once all the threads have been joined, you print
the values from the arrays stored by each thread h.
The atomic variable go c is used to ensure that the threads all start the loop as
near to the same time as possible. Launching a thread is an expensive operation, and
without the explicit delay, the first thread may be finished before the last one has
started. Each thread waits for go to become true before entering the main loop d, f,
and go is set to true only once all the threads have started g.
One possible output from this program is as follows:
(0,0,0),(1,0,0),(2,0,0),(3,0,0),(4,0,0),(5,7,0),(6,7,8),(7,9,8),(8,9,8),
(9,9,10)
(0,0,0),(0,1,0),(0,2,0),(1,3,5),(8,4,5),(8,5,5),(8,6,6),(8,7,9),(10,8,9),
(10,9,10)
(0,0,0),(0,0,1),(0,0,2),(0,0,3),(0,0,4),(0,0,5),(0,0,6),(0,0,7),(0,0,8),
(0,0,9)
(1,3,0),(2,3,0),(2,4,1),(3,6,4),(3,9,5),(5,10,6),(5,10,8),(5,10,10),
(9,10,10),(10,10,10)
(0,0,0),(0,0,0),(0,0,0),(6,3,7),(6,5,7),(7,7,7),(7,8,7),(8,8,7),(8,8,9),
(8,8,9)

There are a few things to notice from this output:
■The first set of values shows x increasing by one with each triplet, the second set
has y increasing by one, and the third has z increasing by one.

■The x elements of each triplet only increase within a given set, as do the y and z
elements, but the increments are uneven, and the relative orderings vary
between all threads.

■Thread 3 doesn’t see any of the updates to x or y; it sees only the updates it
makes to z. This doesn’t stop the other threads from seeing the updates to z
mixed in with the updates to x and y though.
 */
#include <thread>
#include <atomic>
#include <iostream>

std::atomic<int> x(0), y(0), z(0);
std::atomic<bool> go(false);

unsigned const loop_count = 10;

struct read_values
{
	int x, y , z;
};

read_values values1[loop_count];
read_values values2[loop_count];
read_values values3[loop_count];
read_values values4[loop_count];
read_values values5[loop_count];

void increment(std::atomic<int> *var_to_inc, read_values *values)
{
	while(!go)
	{
		std::this_thread::yield();
	}

	for( unsigned int i = 0 ; i < loop_count ; i++ )
	{
		values[i].x = x.load(std::memory_order_relaxed);
		values[i].y = y.load(std::memory_order_relaxed);
		values[i].z = z.load(std::memory_order_relaxed);

		var_to_inc->store(i+1, std::memory_order_relaxed);
		std::this_thread::yield();
	}
}

void read_vals(read_values *values)
{
	while(!go)
	{
		std::this_thread::yield();
	}
	for( unsigned int i = 0 ; i < loop_count ; i++ )
	{
		values[i].x = x.load(std::memory_order_relaxed);
		values[i].y = x.load(std::memory_order_relaxed);
		values[i].z = x.load(std::memory_order_relaxed);
		std::this_thread::yield();
	}
}

void print(read_values *v)
{
	for( int i = 0 ; i < loop_count ; i++ )
	{
		if(i)
		{
			std::cout<<",";
		}
		std::cout<<"("<<v[i].x<<","<<v[i].y<<v[i].z<<")";
	}
	std::cout<<std::endl;
}

int main(int argc, char **argv) {
	std::thread t1(increment, &x, values1);
	std::thread t2(increment, &y, values1);
	std::thread t3(increment, &z, values1);
	std::thread t4(read_vals, values4);
	std::thread t5(read_vals, values5);

	go = true;
	t5.join();
	t4.join();
	t3.join();
	t2.join();
	t1.join();

	print(values1);
	print(values2);
	print(values3);
	print(values4);
	print(values5);

	return 0;
}



































