/*
 * two_mutex_with_deadlock_free.cc
 *
 *  Created on: 07-Feb-2021
 *      Author: prateek
 */
#include <iostream>
#include <thread>
#include <mutex>

std::mutex m1, m2;

void task1()
{
	while(1)
	{
		std::lock(m1, m2);
		std::cout<<"task 1"<<std::endl;
		m1.unlock();
		m2.unlock();
	}
}

void task2()
{
	while(1)
	{
		std::lock(m1, m2);
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



