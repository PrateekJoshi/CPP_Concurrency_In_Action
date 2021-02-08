/*
 * two_mutex_with_deadlock.cc
 *
 *  Created on: 07-Feb-2021
 *      Author: prateek
 *      https://www.youtube.com/watch?v=ruYGFYTRbe8
 *
 *      This program will get stuck because of deadlock.
 */
#include <iostream>
#include <thread>
#include <mutex>

std::mutex m1, m2;

void task1()
{
	while(1)
	{
		m1.lock();
		m2.lock();
		std::cout<<"task 1"<<std::endl;
		m2.unlock();
		m1.unlock();
	}
}

void task2()
{
	while(1)
	{
		m2.lock();
		m1.lock();
		std::cout<<"task 2"<<std::endl;
		m1.unlock();
		m2.unlock();
	}
}

int main(int argc, char **argv) {
	std::thread thread1(task1);
	std::thread thread2(task2);

	thread1.join();
	thread2.join();

	return 0;
}


