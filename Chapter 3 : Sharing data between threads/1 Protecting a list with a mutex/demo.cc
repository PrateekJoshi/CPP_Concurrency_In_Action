/*
 * demo.cc
 *
 *  Created on: 07-Feb-2021
 *      Author: prateek
 *      Pg 38
In C++, you create a mutex by constructing an instance of std::mutex, lock it with a
call to the member function lock(), and unlock it with a call to the member func-
tion unlock(). However, it isn’t recommended practice to call the member functions
directly, because this means that you have to remember to call unlock() on every
code path out of a function, including those due to exceptions. Instead, the Standard
C++ Library provides the std::lock_guard class template, which implements that
RAII idiom for a mutex; it locks the supplied mutex on construction and unlocks it
on destruction, thus ensuring a locked mutex is always correctly unlocked. The fol-
lowing listing shows how to protect a list that can be accessed by multiple threads
using a std::mutex, along with std::lock_guard. Both of these are declared in the
<mutex> header.
 *
 */
#include <list>
#include <mutex>
#include <algorithm>
#include <iostream>

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value)
{
	std::lock_guard<std::mutex> guard(some_mutex);
	some_list.push_back(new_value);
}

bool list_contains(int value_to_find)
{
	std::lock_guard<std::mutex> guard(some_mutex);
	return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}

int main(int argc, char **argv) {
	add_to_list(42);
	std::cout<<"contains(1)="<<list_contains(1)<<", contains(42)="<<list_contains(42)<<std::endl;
	return 0;
}









