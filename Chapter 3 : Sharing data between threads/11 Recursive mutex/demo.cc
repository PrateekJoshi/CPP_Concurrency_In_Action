/*
 * demo.cc
 *
 *  Created on: 08-Feb-2021
 *      Author: prateek
 *      Pg 65
 *      Ref : https://www.youtube.com/watch?v=yCYU2k77E4A
 *
With std::mutex, it’s an error for a thread to try to lock a mutex it already owns, and
attempting to do so will result in undefined behavior. However, in some circumstances it
would be desirable for a thread to reacquire the same mutex several times without
having first released it. For this purpose, the C++ Standard Library provides
std::recursive_mutex. It works just like std::mutex, except that you can acquire
multiple locks on a single instance from the same thread. You must release all your
locks before the mutex can be locked by another thread, so if you call lock() three
times, you must also call unlock() three times. Correct use of std::lock_guard
<std::recursive_mutex> and std::unique_lock<std::recursive_mutex> will han-
dle this for you.

Most of the time, if you think you want a recursive mutex, you probably need to
change your design instead.

Output:
prateek@debian:~/Workspace/Cpp_Concurrency_In_Action/Chapter 3 : Sharing data between threads/11 Recursive mutex$ ./demo.o
Id : 1 Buffer : 0
Id : 1 Buffer : 1
Id : 1 Buffer : 2
Id : 1 Buffer : 3
Id : 1 Buffer : 4
Id : 1 Buffer : 5
Id : 1 Buffer : 6
Id : 1 Buffer : 7
Id : 1 Buffer : 8
Id : 1 Buffer : 9
Id : 1 Buffer : 10
Id : 2 Buffer : 11
Id : 2 Buffer : 12
Id : 2 Buffer : 13
Id : 2 Buffer : 14
Id : 2 Buffer : 15
Id : 2 Buffer : 16
Id : 2 Buffer : 17
Id : 2 Buffer : 18
Id : 2 Buffer : 19
Id : 2 Buffer : 20
Id : 2 Buffer : 21
 */

#include <iostream>
#include <thread>
#include <mutex>

std::recursive_mutex m;
int buffer = 0;

void recursion(int id, int loop_for)
{
	if( loop_for < 0 )
	{
		return;
	}

	m.lock();
	std::cout<< "Id : " <<id <<" Buffer : "<< buffer++<< std::endl;
	recursion(id, --loop_for);
	m.unlock();
}

int main(int argc, char **argv) {
	std::thread t1(recursion, 1 , 10);
	std::thread t2(recursion, 2 , 10);

	t1.join();
	t2.join();

	return 0;
}





