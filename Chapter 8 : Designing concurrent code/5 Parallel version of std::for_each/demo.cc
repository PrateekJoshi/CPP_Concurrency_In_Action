/*
 * demo.cc
 *
 *  Created on: 07-Mar-2021
 *      Author: prateek
 *      Pg 255
This algorithm is similar in concept to the parallel version of std::accumulate
described in section 8.4.1, but rather than computing the sum of each element, you
merely have to apply the specified function. Although you might imagine this would
greatly simplify the code, because there’s no result to return, if you wish to pass on
exceptions to the caller, you still need to use the std::packaged_task and std::
future mechanisms to transfer the exception between threads. A sample implementa-
tion is shown here.

The basic structure of the code is identical to that of listing 8.4, which is unsurpris-
ing. The key difference is that the futures vector stores std::future<void> B
because the worker threads don’t return a value, and a simple lambda function that
invokes the function f on the range from block_start to block_end is used for the
task c. This avoids having to pass the range into the thread constructor d. Since
the worker threads don’t return a value, the calls to futures[i].get() e just provide
a means of retrieving any exceptions thrown on the worker threads; if you don’t wish
to pass on the exceptions, you could omit this.
 */

#include <algorithm>
#include <future>
#include <thread>
#include <vector>

/*
 * Similar to lock_guard, you have your custom join_thread class which prevents threads from
 * leaking in case exception occurs before joining them or on scope exit
 */
class join_threads
{
	std::vector<std::thread>& threads;
public:
	explicit join_threads(std::vector<std::thread>& threads_) : threads(threads_)
	{}

	~join_threads()
	{
		for(unsigned long i = 0 ; i < threads.size() ; i++ )
		{
			if( threads[i].joinable() )
			{
				threads[i].join();
			}
		}
	}
};

template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f)
{
	unsigned long const length =std::distance(first, last);
	if( !length )
	{
		return;
	}

	unsigned long const min_per_thread = 25;
	unsigned long const max_threads = ( length + min_per_thread -1 )/ min_per_thread;
	unsigned long const hardware_threads = std::thread::hardware_concurrency();
	unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads:2 , max_threads);
	unsigned long const block_size = length/ num_threads;

	/* futures vector stores std::future<void> because worker thread don't return a value */
	std::vector<std::future<void>> futures(num_threads-1);
	std::vector<std::thread> threads(num_threads-1);
	join_threads joiner(threads);

	Iterator block_start = first;
	for( unsigned long i = 0 ; i < ( num_threads-1) ; i++ )
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);

		// packaged_task having lambda func that invoke function f on the range from block_start to
		// block_end is used for the task
		std::packaged_task<void(void)> task(
				[=](){
						std::for_each(block_start, block_end, f);
					 });
		futures[i] = task.get_future();
		threads[i] = std::thread(std::move(task));
		block_start = block_end;
	}

	std::for_each(block_start, last, f);

	/* Since worker threads don't return a value, the calls to future[i].get() just provide a means of
	 * retrieving any exception thrown on worker threads.
	 * If you don't wish to pass on the exceptions , you could omit this.
	 */
	for( unsigned long i = 0 ; i < ( num_threads-1); i++ )
	{
		futures[i].get();
	}

}



