/*
 * demo.cc
 *
 *  Created on: 11-Mar-2021
 *      Author: prateek
 *      Pg 260
The desire to finish early if you find a match means that you need to introduce a flag
that is shared between all threads to indicate that a match has been found. This there-
fore needs to be passed in to all recursive calls. The simplest way to achieve this is by
delegating to an implementation function B that takes an additional parameter—a
reference to the done flag, which is passed in from the main entry point 1@.

The core implementation then proceeds along familiar lines. In common with
many of the implementations here, you set a minimum number of items to process on
a single thread c; if you can’t cleanly divide into two halves of at least that size, you
run everything on the current thread d. The actual algorithm is a simple loop
through the specified range, looping until you reach the end of the range or the done
flag is set e. If you do find a match, the done flag is set before returning f. If you
stop searching either because you got to the end of the list or because another thread
set the done flag, you return last to indicate that no match was found here g.

If the range can be divided, you first find the midpoint h before using std::async
to run the search in the second half of the range i , being careful to use std::ref to
pass a reference to the done flag. In the meantime, you can search in the first half of
the range by doing a direct recursive call j. Both the asynchronous call and the
direct recursion may result in further subdivisions if the original range is big enough.
If the direct search returned mid_point, then it failed to find a match, so you need
to get the result of the asynchronous search. If no result was found in that half, the
result will be last, which is the correct return value to indicate that the value was not
found 1). If the “asynchronous” call was deferred rather than truly asynchronous, it
will actually run here in the call to get(); in such circumstances the search of the top
half of the range is skipped if the search in the bottom half was successful. If the asyn-
chronous search is really running on another thread, the destructor of the
async_result variable will wait for the thread to complete, so you don’t have any leak-
ing threads.

As before, the use of std::async provides you with exception-safety and exception-
propagation features. If the direct recursion throws an exception, the future’s destruc-
tor will ensure that the thread running the asynchronous call has terminated before
the function returns, and if the asynchronous call throws, the exception is propagated
through the get() call 1) . The use of a try/catch block around the whole thing is only
there to set the done flag on an exception and ensure that all threads terminate quickly
if an exception is thrown 1!. The implementation would still be correct without it but
would keep checking elements until every thread was finished.
 */
#include <atomic>
#include <future>

template<typename Iterator, typename MatchType>
Iterator parallel_find_impl(Iterator first, Iterator last, MatchType match, std::atomic<bool>& done)
{
	try
	{
		unsigned long const length = std::distance(first, last);

		// Set a min no of items to process on a single thread, if you can't cleanly divide into the
		// two halves of at least that size, you run everything on the current thread
		unsigned long const min_per_thread = 25;
		if( length < (2*min_per_thread) )
		{
			/* Loop until you reach the end of the range or the done flag is set */
			for( ; (first != last) && !done.load() ; first++ )
			{
				if( *first == match )
				{
					// If you find a match, done flag is set before returning
					done = true;
					return first;
				}
				return last;
			}
		}
		else
		{
			Iterator const mid_point = first + (length/2);

			// std::async searches in the second half
			// NOTE : Careful to use std::ref to pass reference to the done flag
			// 1. If direct recursion throws an exception, the future's destructor will ensure
			// that the thread running async call has terminated before function returns.
			// 2. If async call throws, the exception is propagated through the get() call
			std::future<Iterator> async_result = std::async(&parallel_find_impl<Iterator,MatchType>,
															mid_point, last, match, std::ref(done));

			// Search first half by doing a direct recursive call
			Iterator const direct_result = parallel_find_impl(first, last, match, done);

			/* If direct serach returned mid_point, then it failed to find a match, so you need to get
			 * the result of async search. If no result will be find in that half, the result will be last
			 * which is the correct return value to indicate result was not found.
			 */
			return ( direct_result == mid_point ) ? async_result.get() : direct_result;
		}
	}
	catch(...)
	{
		/*
		 * Use of try/catch around whole thing is only there to set done flag on an exception and
		 * ensure all threads terminate quickly if an exception is thrown.
		 * The impl would still be correct without it but would keep checking elements until every
		 * thread was finished.
		 */
		done = true;
		throw;
	}
}

template<typename Iterator, typename MatchType>
Iterator parallel_find(Iterator first, Iterator last, MatchType match)
{
	std::atomic<bool> done(false);
	return parallel_find_impl(first, last, match , done);
}



