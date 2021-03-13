/*
 * demo.cc
 *
 *  Created on: 13-Mar-2021
 *      Author: prateek
 *      Pg 264
 */

#include <algorithm>
#include <future>
#include <thread>
#include <vector>
#include <numeric>

/*
 * Thread guard
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

template<typename Iterator>
void parallel_partial_sum(Iterator first, Iterator last)
{
	typedef typename Iterator::value_type value_type;

	//Actual processing function
	struct process_chunk
	{
		void operator () (Iterator begin, Iterator last,
							std::future<value_type>* previous_end_value,
							std::promise<value_type>* end_value)
		{
			try
			{
				Iterator end = last;
				++end;

				// Call std::partial_sum for entire chunk, including the final element
				std::partial_sum(begin, end, begin);

				// Check if you are the first chunk or not.
				if( previous_end_value )
				{
					/*
					 * If you are not the first chunk, there was a previous_end_value
					 from the previous chunk, so you need to wait for that
					 */
					value_type& add_end = previous_end_value->get();

					/* In order to maximize parallelism of the algo, you then update the last element first, so you can pass the
					 * value on to next chunk ( if there is one) : set_value on promise end_value
					 */
					*last += add_end;

					if(end_value)
					{
						end_value->set_value(*last);
					}

					/*
					 * Use std::for_each and a simple lambda function to update all the remaining elements in the range
					 */
					std::for_each(begin, last, [add_end](value_type& item)
										{
											item += add_end;
										});
				}
				else if( end_value )
				{
					// If first block ( no prev end value to wait)
					end_value->set_value(*last);
				}
			}
			catch (...)
			{
				/*
				 * If any of the operations threw an exception, you catch it and store it in the promise so it will propagate to next chunk when it tries to get the
				 * prev end value. This will propagate exception to final chunk, which then just rethrows, because you know yo are running on main thread
				 */
				if( end_value )
				{
					// store exception in promise
					end_value->set_exception(std::current_exception());
				}
				else
				{
					// rethrow exception ( only main thread will be here *)/
					throw;
				}
			}
		}

	};

	unsigned long const length=std::distance(first,last);

	if (!length)
	{
		return ;
	}

	unsigned long const min_per_thread = 25;
	unsigned long const max_threads = (length + min_per_thread - 1)/ min_per_thread;
	unsigned long const hardware_threads = std::thread::hardware_concurrency();
	unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
	unsigned long const block_size = length / num_threads;

	typedef typename Iterator::value_type value_type;

	//Vcetor of threads
	std::vector<std::thread> threads(num_threads-1);

	// vector of promises : Used to store last value from the previous chunk
	std::vector<std::promise<value_type>> end_values(num_threads-1);

	//vector of futures : Used to retrieve last value from the previous chunk
	std::vector<std::future<value_type>> previous_end_values;

	//thread guard wrapper ( joining in case of any exception or when out of scope)
	join_threads joiner(threads);

	Iterator block_start = first;

	for( unsigned long i = 0 ; i < ( num_threads-1) ; ++i )
	{
		Iterator block_last = block_start;
		std::advance(block_last, block_size-1);

		/*
		 * 1. Actual processing is done in process_chunk function
		 * 2. Start and end iterators for this chunk are passed in as arguments alongside the future for the end value of prev range( if any)
		 * 3. And promise to hold end value of this range
		 */
		threads[i] = std::thread(process_chunk(),
								block_start,
								block_last,
								(i!=0) ? &previous_end_values[i-1] : 0
								&end_values[i]);

		block_start = block_last;
		++block_start;

		//Store the future for the last value in the current chunk into the vector of futures so it will be picked up next time around the loop
		previous_end_values.push_back(end_values[i].get_future());
	}

	Iterator final_element = block_start;
	std::advance(final_element, std::distance(block_start, last)-1);

	process_chunk()(block_start,
					final_element,
					(num_threads > 1) ? &previous_end_values.back() : 0,
					0);
}


