/*
 * demo.cc
 *
 *  Created on: 09-Feb-2021
 *      Author: prateek
 *      Pg : 77
You use std::async to start an asynchronous task for which you don’t need the
result right away. Rather than giving you back a std::thread object to wait on,
std::async returns a std::future object, which will eventually hold the return value
of the function. When you need the value, you just call get() on the future, and the
thread blocks until the future is ready and then returns the value. The following listing
shows a simple example.
 *
 */

#include <future>
#include <iostream>

int find_the_answer_to_ltuae()
{
	return 42;
}

void do_other_stuff()
{}

int main(int argc, char **argv) {
	std::future<int> answer = std::async(find_the_answer_to_ltuae);
	do_other_stuff();
	std::cout<<"The answer is "<<answer.get()<<std::endl;
	return 0;
}



