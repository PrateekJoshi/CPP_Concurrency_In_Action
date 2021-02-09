/*
 * demo.cc
 *
 *  Created on: 09-Feb-2021
 *      Author: prateek
 *      https://youtu.be/FfbZfBk-3rI?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
 */

#include <deque>
#include <future>
#include <iostream>
#include <functional>
#include <condition_variable>

int factorial(int n)
{
	int result = 1;
	for( int i = n ; i > 1 ; i-- )
	{
		result *= i;
	}

	return result;
}

std::deque< std::packaged_task<int()> > task_q;		// Task queue
std::mutex mu;										// mutex to prevent race condition during access by multiple threads
std::condition_variable cond;						// To make sure task_q.pop_front() is not called if queue is empty ( otherwise we will have runtime error)

void thread_1()
{
	// pop the packaged task
	std::packaged_task<int()> t;

	{
		// lock the mutex before accessing task queue
		std::unique_lock<std::mutex> lck(mu);

		// Wait till task queue is empty
		cond.wait(lck, [](){
			return !task_q.empty();
		});

		t = std::move(task_q.front());
		task_q.pop_front();
	}

	//Execute the task in thread 1
	t();
}

int main(int argc, char **argv) {
	/* Create another thread which will pop the task from task queue and execute it */
	std::thread t1(thread_1);

	/* Create a packaged task */
	// As we have binded the arg with function factorial the function signature in packaged task is int (<no_arg>)
	std::packaged_task<int()> task(std::bind(factorial, 6));

	// Associate a future with it so that we can gets the task result later on
	std::future<int> fu = task.get_future();

	{
		// lock the mutex before accessing task queue
		std::lock_guard<std::mutex> lck(mu);

		//Push the task to the queue
		// Since the task is no longer needed in the main thread , just move it to the task queue
		task_q.push_back(std::move(task));
	}

	// notify cond variable that something is pushed to the task queue
	cond.notify_one();

	// Get the result of the executed task in main thread
	std::cout<<"Factorial is :"<<fu.get()<<std::endl;

	t1.join();


	return 0;
}





