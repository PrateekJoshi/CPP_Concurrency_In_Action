/*
 * demo.cc
 *
 *  Created on: 09-Feb-2021
 *      Author: prateek
 *      https://www.youtube.com/watch?v=SZQ6-pf-5Us
 *
 */
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

int factorial(std::future<int> &fut)
{
	int result = 1;

	int n = fut.get();			// get the value from future if available ( else wait )

	for( int i = n ; i > 1 ; i-- )
	{
		result *= i;
	}

	return result;
}

int main(int argc, char **argv) {
	std::promise<int> prom;								// Create a promise
	std::future<int> fut = prom.get_future();			// Assign future from the promise

	// Launch aysn thread to run factorial() and pass the future as reference to this thread
	std::future<int> factorial_future = std::async(std::launch::async, factorial, std::ref(fut));

	// Do something else
	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	//Since we have kept the promise , now its time to fulfil it (Pass value to async thread)
	prom.set_value(4);

	// Get the value from the async thread future  ( Get value from async thread )
	int x = factorial_future.get();

	std::cout<<"Result is : "<<x<<std::endl;

	return 0;
}





