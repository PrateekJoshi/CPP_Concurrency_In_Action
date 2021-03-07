/*
 * demo.cc
 *
 *  Created on: 04-Mar-2021
 *      Author: prateek
 *      Pg 228
 */

#include <boost/smart_ptr/shared_ptr.hpp>
#include <atomic>
#include <future>
#include <list>
#include <thread>
#include <vector>

#include "thread_safe_stack.cc"

template<typename T>

/* Sorter class */
struct sorter
{
	/* Data type representing a chunk to sort and associated promise object */
	struct chunk_to_sort
	{
		std::list<T> data;
		std::promise<std::list<T>> promise;
	};

	/* A stack containing of unsorted chunks */
	threadsafe_stack<chunk_to_sort> chunks;

	/* A set of threads */
	std::vector<std::thread> threads;

	/* Max thread count */
	unsigned const max_thread_count;

	/* Flag to indicate threads can finish */
	std::atomic<bool> end_of_data;

	/* Constructor */
	sorter() : max_thread_count(std::thread::hardware_concurrency()-1), end_of_data(false)
	{}

	/* Destructor */
	~sorter()
	{
		end_of_data = true;
		for( unsigned i = 0 ; i < threads.size() ; i++ )
		{
			threads[i].join();
		}
	}

	/* Pop a chunk from the stack and sort it */
	void try_sort_chunk()
	{
		/* Pop a chunk */
		boost::shared_ptr<chunk_to_sort> chunk = chunks.pop();
		if( chunk )
		{
			/* Sort it */
			sort_chunk(chunk);
		}
	}

	/* Sort the chunk and set the promise on the chunk once sorted */
	void sort_chunk(boost::shared_ptr<chunk_to_sort> const &chunk)
	{
		/*
		 * 1. Sort the chunk do_sort()
		 * 2.Store the result in the promise , ready to be picked up by the thread that posted the chunk on the
		 *   stack.
		 */
		chunk->promise.set_value(do_sort(chunk->data));
	}

	/* Main sorting function
	 * 1. Partition the data
	 * 2. Push lower chunk to the stack.
	 * 3. Spawn a new thread to process.
	 * 4. As lower chunk might me handled by another thread, you wait wait for it to be ready.
	 * 5. try to process chunks from stack on this thread while you are waiting.
	 * 6. Once lower chunk is sorted (ready), you store the result and return it.
	 */
	std::list<T> do_sort(std::list<T>& chunk_data)
	{
		if( chunk_data.empty() )
		{
			return chunk_data;
		}

		std::list<T> result; 		// To store sorted result

		/* Store first element of chunk_data in result */
		result.splice(result.begin(), chunk_data, chunk_data.begin());

		/* Partition the chunk_data along value stored in result[0] */
		T const& partition_val = *result.begin();

		typename std::list<T>::iterator divide_point = std::partition(chunk_data.begin(), chunk_data.end(),
					[&](T const& val)
					{
							return val < partition_val;
					});

		/* Copy lower data chunk */
		chunk_to_sort new_lower_chunk;
		new_lower_chunk.data.splice(new_lower_chunk.data.end(),		// copy here
									chunk_data, 					// copy from
									chunk_data.begin(),				// start iterator to copy
									chunk_data.end());				// end iterator to copy

		/* Store future of the new lower , later we can check status using future object whether it
		 * is sorted or not
		 */
		std::future<std::list<T>> new_lower_future = new_lower_chunk.promise.get_future();

		/* Push the new lower chunk to thread safe stack */
		chunks.push(std::move(new_lower_chunk));

		/* If no of threads < harware concurrency, then spawn a new thread and add to threads list */
		if( threads.size() < max_thread_count )
		{
			threads.push_back(std::thread(&sorter<T>::sort_thread, this));
		}

		/* Sort the higher data chunk in current thread */
		std::list<T> new_higher(do_sort(chunk_data));

		/* Append new_higher sorted chunk to end of result */
		result.splice(result.end(), new_higher);

		/* Check on new_lower future to see if it is ready (sorted) */
		while( new_lower_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready )
		{
			/* lower is not ready , so instead of current thread being waiting for lower chunk future
			 * we try to utilize the current thread pop a data_chunk to be sorted from stack and do
			 * sort
			 */
			try_sort_chunk();
		}

		/* Add new_lower chunk to begining of result list */
		result.splice(result.begin(), new_lower_future.get());

		/* Return sorted result */
		return result;
	}

	/*
	 *  Loop while end of data is set:
	 *  	1. Pick a data chunk to sort from the stack and sort it
	 *  	2. Yield to other threads to give them a chance to put some more work on the stack
	 */
	void sort_thread()
	{
		while( !end_of_data )
		{
			//Pick a data chunk to sort from the stack and sort it
			try_sort_chunk();
			std::this_thread::yield();
		}
	}

};


/* Entry point for parallel quick sort */
template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
	if (input.empty())
	{
		return input;
	}

	/* Create a sorter class object */
	sorter<T> s;

	return s.do_sort(input);
}



