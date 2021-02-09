/*
 * demo.cc
 *
 *  Created on: 10-Feb-2021
 *      Author: prateek
 *      Pg 91
For example,
if you have a maximum of 500 milliseconds to wait for an event associated with a con-
dition variable, you might do something like in the following listing.

This is the recommended way to wait for condition variables with a time limit, if you’re
not passing a predicate to the wait. This way, the overall length of the loop is bounded.
As you saw in section 4.1.1, you need to loop when using condition variables if you
don’t pass in the predicate, in order to handle spurious wakeups. If you use
wait_for() in a loop, you might end up waiting almost the full length of time before
a spurious wake, and the next time through the wait time starts again. This may repeat
any number of times, making the total wait time unbounded.

 *
 */
#include <condition_variable>
#include <mutex>
#include <chrono>

std::condition_variable cv;
bool done;
std::mutex mu;

bool wait_loop()
{
	auto const timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);		// time point
	std::unique_lock<std::mutex> lk(mu);

	while(!done)
	{
		if( cv.wait_until(lk, timeout) == std::cv_status::timeout )
		{
			break;
		}
	}

	return done;
}







