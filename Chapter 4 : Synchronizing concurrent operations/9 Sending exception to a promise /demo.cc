/*
 * demo.cc
 *
 *  Created on: 09-Feb-2021
 *      Author: prateek
 *      https://www.youtube.com/watch?v=SZQ6-pf-5Us
 */
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <exception>

int factorial(std::future<int> &fut)
{
	int result = 1;

	try
	{
		int n = fut.get();			// here we will receive exception thrown from the other thread
		for( int i = n ; i > 1 ; i-- )
		{
			result *= i;
		}
	}
	catch (std::runtime_error &err) {
		std::cout<<err.what()<<std::endl;
		return -1;					// in case of exception return -1
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

	//Since we have kept the promise but we now know we can't fulfil it. So send an exception to the promise
	//so that other thread will get the exception
	prom.set_exception(std::make_exception_ptr(std::runtime_error("Can't fulfil promise, user defined exception")));

	// Get the value from the async thread future  ( Get value from async thread )
	int x = factorial_future.get();

	std::cout<<"Result is : "<<x<<std::endl;

	return 0;
}




