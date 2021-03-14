/*
 * demo.cc
 *
 *  Created on: 13-Mar-2021
 *      Author: prateek
 *      Pg 275
At its simplest, a thread pool is a fixed number of worker threads (typically the same
number as the value returned by std::thread::hardware_concurrency()) that pro-
cess work. When you have work to do, you call a function to put it on the queue of
pending work. Each worker thread takes work off the queue, runs the specified task,
and then goes back to the queue for more work. In the simplest case there’s no way to
wait for the task to complete. If you need to do this, you have to manage the synchro-
nization yourself.
*/

#include <thread>
#include <vector>
#include <atomic>

/* Implementation in other chapter */
struct join_threads
{
    join_threads(std::vector<std::thread>&)
    {}
};

class thread_pool
{
	/*
	 * 1. thread_safe_queue to manage queue of work.
	 * 2. join_thread class from chapter 8 to join all threads
	 * 3. NOTE : Order of declaration of the members is important : both the done flag and the worker_queue
	 *    must be declared before the threads vector, which must in turn be declared before the joiner.
	 *	  This ensures that the members are destroyed in the right order, you can't destroy the queue safely
	 *	  until all the threads have stopped.
	 */
	std::atomic_bool done;
	thread_safe_queue<std::function<void()>> work_queue;
	std::vector<std::thread> threads;
	join_threads joiner;

	void worker_thread()
	{
		/* Sit in loop waiting until the done flag is set */
		while( !done )
		{
			std::function<void()> task;

			// Pull task of the queue
			if( work_queue.try_pop(task) )
			{
				// execute task
				task();
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}


public:
	/* Constructor */
	thread_pool() : done(false), joiner(threads)
	{
		// Get hardware concurrency count
		unsigned const thread_count = std::thread::hardware_concurrency();

		try
		{
			// Create threads
			for( unsigned i = 0 ; i < thread_count ; i++ )
			{
				threads.push_back(std::thread(&thread_pool::worker_thread, this));
			}
		}
		catch(...)
		{
			/*
			 * Thread can fail by throwing an exception, so you need to ensure that any threads yo have
			 * started are stopped and cleaned up nicely in this case. This is achieved with a try-catch
			 * block that sets the done flag when an exception is thrown.
			 */
			done = true;
			throw;
		}
	}

	/* Destructor */
	~thread_pool()
	{
		done = true;
	}

	/* Submit a task to work queue */
	template<typename FunctionType>
	void submit(FunctionType f)
	{
		work_queue.push(std::function<void()>(f));
	}

};




