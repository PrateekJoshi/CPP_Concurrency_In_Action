/*
 * demo.cc
 *
 *  Created on: 07-Mar-2021
 *      Author: prateek
 *      Pg : 257
As with your std::async-based parallel_accumulate from listing 8.5, you split the
data recursively rather than before execution, because you don’t know how many
threads the library will use. As before, you divide the data in half at each stage, run-
ning one half asynchronously c and the other directly d until the remaining data is
too small to be worth dividing, in which case you defer to std::for_each B. Again,
the use of std::async and the get() member function of std::future e provides the
exception propagation semantics.
 */
#include <future>
#include <algorithm>

template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f)
{
	unsigned long const length = std::distance(first, last);
	if( !length )
	{
		return;
	}

	unsigned long const min_per_thread = 25;
	if( length < (2*min_per_thread) )
	{
		std::for_each(first, last, f);
	}
	else
	{
		Iterator const mid_point = first + (length/2);

		// Using std::async and get() member function of std::future provide exception propagation semantics
		std::future<void> first_half = std::async(&parallel_for_each<Iterator, Func>,first, last, f);

		parallel_for_each(mid_point, last, f);

		first_half.get();
	}
}




