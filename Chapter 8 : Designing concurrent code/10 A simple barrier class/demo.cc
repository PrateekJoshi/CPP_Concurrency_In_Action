/*
 * demo.cc
 *
 *  Created on: 13-Mar-2021
 *      Author: prateek
 *      Pg 267
 *
Imagine a roller coaster at the fairground. If there’s a reasonable number of peo-
ple waiting, the fairground staff will ensure that every seat is filled before the roller
coaster leaves the platform. A barrier works the same way: you specify up front the
number of “seats,” and threads have to wait until all the “seats” are filled. Once there
are enough waiting threads, they can all proceed; the barrier is reset and starts waiting
for the next batch of threads. Often, such a construct is used in a loop, where the
same threads come around and wait next time. The idea is to keep the threads in lock-
step, so one thread doesn’t run away in front of the others and get out of step. For an
algorithm such as this one, that would be disastrous, because the runaway thread
would potentially modify data that was still being used by other threads or use data
that hadn’t been correctly updated yet.

 */
#include <thread>
#include <atomic>

class barrier
{
	unsigned const count;
	std::atomic<unsigned> spaces;
	std::atomic<unsigned> generation;

public:
	/* Initially no of spaces will be equal to no of count */
	explicit barrier(unsigned count_) : count(count_), spaces(count), generation(0)
	{}

	void wait()
	{
		// store generation value initially
		unsigned const my_generation = generation;

		// As each threads call wait(), spaces decremented
		if( !--spaces )
		{
			// When it reaches zero, the no of spaces is reset back to count
			// and generation is increased to signal to the other threads that they can continue
			spaces = count;
			++generation;
		}
		else
		{
			/*1. If the no of spaces does not reach zero, then you have to wait.
			  2. This implementation uses a simple spin lock, checking the generation aganst value you retieved at beginning
			 of wait.
			  3. Because the generation is only updated when all the threads have reached the barrier, you yield() while waiting so
			    the waiting thread doesn't hog the CPU in a busy wait
			 */
			while( generation == my_generation )
			{
				std::this_thread::yield();
			}
		}

	}
};




