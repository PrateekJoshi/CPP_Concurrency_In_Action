/*
 * demo.cc
 *
 *  Created on: 06-Mar-2021
 *      Author: prateek
 *      Pg 249
This version uses recursive division of the data rather than pre-calculating the division
of the data into chunks, but it’s a whole lot simpler than the previous version, and it’s
still exception safe. As before, you start by finding the length of the sequence B, and if
it’s smaller than the maximum chunk size, you resort to calling std::accumulate
directly c. If there are more elements than your chunk size, you find the midpoint d
and then spawn an asynchronous task to handle that half e. The second half of the
range is handled with a direct recursive call f, and then the results from the two
chunks are added together g. The library ensures that the std::async calls make use
of the hardware threads that are available without creating an overwhelming number of
threads. Some of the “asynchronous” calls will actually be executed synchronously in
the call to get().

The beauty of this is that not only can it take advantage of the hardware concur-
rency, but it’s also trivially exception safe. If an exception is thrown by the recursive
call f, the future created from the call to std::async e will be destroyed as the
exception propagates. This will in turn wait for the asynchronous task to finish, thus
avoiding a dangling thread. On the other hand, if the asynchronous call throws, this is
captured by the future, and the call to get() g will rethrow the exception.
*/
#include <future>
#include <algorithm>

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
	unsigned long const length = std::distance(first, last);
	unsigned long const max_chunk_size = 25;

	// If length is less than max chunk size, call std::accumulate directly
	if( length <= max_chunk_size )
	{
		return std::accumulate(first, last, init);
	}
	else
	{
		Iterator mid_point = first;

		//If there are more elements than your chunk size, you find the midpoint and then spawn
		//an async task to handle that half
		std::advance(mid_point, length/2);

		//std::async call make use of hardware threads that are available without creating an overwhelming
		//number of threads + exception safe
		std::future<T> first_half_result = std::async(parallel_accumulate<Iterator,T>, first,
														mid_point, init);

		// second half range is handled with direct recursive call
		T second_half_result = parallel_accumulate(mid_point, last, T());

		return first_half_result.get() + second_half_result;
	}
}




