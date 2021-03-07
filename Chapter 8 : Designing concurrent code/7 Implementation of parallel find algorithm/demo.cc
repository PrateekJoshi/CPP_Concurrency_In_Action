/*
 * demo.cc
 *
 *  Created on: 07-Mar-2021
 *      Author: prateek
 *      Pg 258
The main body of listing 8.9 is similar to the previous examples. This time, the work is
done in the function call operator of the local find_element class B. This loops
through the elements in the block it’s been given, checking the flag at each step c. If
a match is found, it sets the final result value in the promise d and then sets the
done_flag e before returning.

If an exception is thrown, this is caught by the catchall handler f, and you try to
store the exception in the promise g before setting the done_flag. Setting the value
on the promise might throw an exception if the promise is already set, so you catch
and discard any exceptions that happen here h.

This means that if a thread calling find_element either finds a match or throws an
exception, all other threads will see done_flag set and will stop. If multiple threads
find a match or throw at the same time, they’ll race to set the result in the promise.
But this is a benign race condition; whichever succeeds is therefore nominally “first”
and is therefore an acceptable result.

Back in the main parallel_find function itself, you have the promise i and
flag j used to stop the search, both of which are passed in to the new threads
along with the range to search 1!. The main thread also uses find_element to
search the remaining elements 1@. As already mentioned, you need to wait for all
threads to finish before you check the result, because there might not be any
matching elements. You do this by enclosing the thread launching-and-joining code
in a block 1), so all threads are joined when you check the flag to see whether a
match was found 1#. If a match was found, you can get the result or throw the
stored exception by calling get() on the std::future<Iterator> you can get from
the promise 1$.
 */

#include <atomic>
#include <future>
#include <thread>
#include <vector>

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

template<typename Iterator, typename MatchType>
Iterator parallel_find(Iterator first, Iterator last, MatchType match)
{
	/* Work is done in the function call operator of the local find_element class */
	struct find_element
	{
		void operator()(Iterator begin,
						Iterator end,
						MatchType match,
						std::promise<Iterator> *result,
						std::atomic<bool> *done_flag)
		{
			try
			{
				// Loops through the elements in the block its been given, checking the flag at each
				// step
				for( ; (begin != end) && !done_flag->load() ; begin++ )
				{
					if( *begin == match )
					{
						// If a match is found, it sets the final result value in the promise and then
						// sets the done_flag before returning
						result->set_value(begin);
						done_flag->store(true);
						return;
					}
				}
			}
			//If an exception is thrown, this is caught by catchall handler
			catch(...)
			{
				// Try to store exception in a promise, before setting done_flag
				// Setting the value on the promise might throw an exception if promise is already set, so
				// you catch and discard any exceptions that happen here
				try
				{
					result->set_exception(std::current_exception());
					done_flag->store(true);
				}
				catch(...)
				{}
			}
		}

	};

	unsigned long const length = std::distance(first, last);

	if (!length)
	{
		return last;
	}

	unsigned long const min_per_thread = 25;
	unsigned long const max_threads = (length + min_per_thread - 1)/ min_per_thread;

	unsigned long const hardware_threads = std::thread::hardware_concurrency();

	unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

	unsigned long const block_size = length / num_threads;

	// promise and flag used to stop the search , both of which are passed in to the new threads
	// along with range of search
	std::promise<Iterator> result;
	std::atomic<bool> done_flag(false);
	std::vector<std::thread> threads(num_threads-1);
	{
		// thread joiner at end of {} scope
		join_threads joiner(threads);
		Iterator block_start = first;

		for(unsigned long i = 0 ; i < ( num_threads-1) ; i++ )
		{
			Iterator block_end = block_start;
			std::advance(block_end, block_size);

			// launch thread
			threads[i] = std::thread(find_element(), block_start, block_end, match, &result, &done_flag);

			block_start = block_end;
		}

		// call find_element on main thread
		find_element()(block_start, last, match, &result, &done_flag);
	}// At this point all THREADS WILL BE JOINED

	// Check if a match is found or not
	if( !done_flag.load() )
	{
		return last;	// not found
	}

	// If a match was found, you can get the result or throw the stored exception by calling get()
	// on std::future<Iteartor> you can get from the promise
	return result.get_future().get();
}





