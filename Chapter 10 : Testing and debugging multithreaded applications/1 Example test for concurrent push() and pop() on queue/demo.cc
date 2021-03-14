/*
 * demo.cc
 *
 *  Created on: 14-Mar-2021
 *      Author: prateek
 *      Pg 312
 */

#include <future>

void test_concurrent_push_and_pop_on_empty_queue()
{
	// Create your empty queue as part of the general setup
	threadsafe_queue<int> q;

	// Create all your promises for the ready signals
	std::promise<void> go, push_ready, pop_ready;

	// Get a std::shared_future for the go signal
	std::shared_future<void> ready(go.get_future());

	// Create the futures you will use to indicate that the threads have finished
	/*
	 * These have to go outside the try block so that you can set the go signal on an exception
	 * without waiting for the test threads to complete (which would deadlock).
	 */
	std::future<void> push_done;
	std::future<int> pop_done;

	try
	{
		/*
		 * 1. std::launch::async to guarantee that the tasks are each running on their own thread.
		 * 2. std::aysnc makes your exception-safety task easier than it would be plain std::thread because the
		 *    destructor for the future will join with the thread.
		 */
		push_done = std::async(std::launch::async,
								[&q, ready, &push_ready]()
								{
									// Set promise that push thread is ready
									push_ready.set_value();

									// wait for ready signal from main thread. reday is a shared future
									// so multiple threads can listen on it
									ready.wait();

									//Push to queue
									q.push(42);
								});

		pop_done = std::async(std::launch::async,
								[&q, ready, &pop_ready]()
								{
									// Set promise that pop thread is ready
									pop_ready.set_value();

									// wait for ready signal from main thread. reday is a shared future
									// so multiple threads can listen on it
									ready.wait();

									//Pop from queue and return to pop_done future
									return q.pop();
								});

		/* Main thread : waiting for the signals from both threads before signaling them to start the real test */
		push_ready.get_future().wait();
		pop_ready.get_future().wait();

		//start the test
		go.set_value();

		/*
		 * pop thread returns the retieved value through the future, so you use that to get the result for
		 * the assert
		 */
		assert(pop_done.get() == 42);
	}
	catch(...)
	{
		/*
		 * 1. If an exception is thrown, you set the go signal to avoid any chance of a dangling thread and retrow
		 * the exception.
		 * 2. The futures corresponding to the tasks were declared last, so they will be destroyed first, and their
		 *  destructors will wait for the tasks(thread) to complete if they haven't already.
		 */
		go.set_value();
		throw;
	}



}




