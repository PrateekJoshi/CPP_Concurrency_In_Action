/*
 * demo.cc
 *
 *  Created on: 09-Feb-2021
 *      Author: prateek
 *      https://www.youtube.com/watch?v=eTMY5zXits8
 */
#include <iostream>
#include <future>


typedef unsigned long long int ull;

ull find_odd(ull start, ull end)
{
	std::cout<<"find_odd Thread id : "<<std::this_thread::get_id()<<std::endl;
	ull odd_sum = 0;
	for( ull i = start ; i <= end ; i++ )
	{
		odd_sum += i;
	}

	return odd_sum;
}

int main(int argc, char **argv) {
	ull start = 0 , end = 1900000000;

	std::cout<<"Thread created if policy is std::launch::async"<<std::endl;
	std::cout<<"main Thread id : "<<std::this_thread::get_id()<<std::endl;
	std::future<ull> odd_sum = std::async(std::launch::async, find_odd, start , end);

	std::cout<<"Waiting for result"<<std::endl;
	std::cout<<"odd_sum : "<< odd_sum.get()<<std::endl;

	return 0;
}





