/*
 * demo.cc
 *
 *  Created on: 09-Feb-2021
 *      Author: prateek
 *      https://www.youtube.com/watch?v=SZQ6-pf-5Us
 *
 *      Useful for broadcast kind of communication model
 */
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

int factorial(std::shared_future<int> fut)
{
	int result = 1;

	int n = fut.get();			// get the value from future if available ( else wait )

	for( int i = n ; i > 1 ; i-- )
	{
		result *= i;
	}

	std::cout<<"Result is "<< result<<std::endl;

	return result;
}

int main(int argc, char **argv) {
	std::promise<int> prom;								// Create a promise
	std::future<int> fut = prom.get_future();			// Assign future from the promise
	std::shared_future<int> shared_fut = fut.share();	// Get a future which we can share among various threads

	// Launch aysn thread to run factorial() and pass the future copy ( not ref in case of shared_future)
	// to series of threads
	std::future<int> factorial_future1 = std::async(std::launch::async, factorial, shared_fut);		// Thread 1
	std::future<int> factorial_future2 = std::async(std::launch::async, factorial, shared_fut);		// Thread 2
	std::future<int> factorial_future3 = std::async(std::launch::async, factorial, shared_fut);		// Thread 3
	std::future<int> factorial_future4 = std::async(std::launch::async, factorial, shared_fut);		// Thread 4

	// Do something else
	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	//Since we have kept the promise , now its time to fulfil it (Pass value to async thread)
	// All 4 threads will receive the same promise ( value 4 )
	prom.set_value(4);

	return 0;
}




