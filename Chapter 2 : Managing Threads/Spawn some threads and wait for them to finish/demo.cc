/*
 * demo.cc
 *
 *  Created on: 07-Feb-2021
 *      Author: prateek
 *      Pg 28
 */
#include <algorithm>
#include <functional>
#include <thread>
#include <vector>

void do_work(int id) {}

int main(int argc, char **argv) {
	std::vector<std::thread> threads;
	for( int i = 0 ; i < 20 ; i++ )
	{
		threads.push_back(std::thread(do_work,i));
	}

	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}




