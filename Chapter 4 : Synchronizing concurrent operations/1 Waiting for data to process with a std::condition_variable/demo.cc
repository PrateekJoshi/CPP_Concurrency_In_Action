/*
 * demo.cc
 *
 *  Created on: 08-Feb-2021
 *      Author: prateek
 *      Pg 69
First off, you have a queue B that’s used to pass the data between the two threads.
When the data is ready, the thread preparing the data locks the mutex protecting the
queue using a std::lock_guard and pushes the data onto the queue c. It then calls
the notify_one() member function on the std::condition_variable instance to
notify the waiting thread (if there is one) d.

On the other side of the fence, you have the processing thread. This thread first
locks the mutex, but this time with a std::unique_lock rather than a std::lock_
guard e—you’ll see why in a minute. The thread then calls wait() on the std::
condition_variable, passing in the lock object and a lambda function that expresses
the condition being waited for f. Lambda functions are a new feature in C++11 that
allows you to write an anonymous function as part of another expression, and they’re
ideally suited for specifying predicates for standard library functions such as wait().
In this case, the simple lambda function []{return !data_queue.empty();} checks
to see if the data_queue is not empty()—that is, there’s some data in the queue ready
for processing. Lambda functions are described in more detail in appendix A, sec-
tion A.5.

The implementation of wait() then checks the condition (by calling the supplied
lambda function) and returns if it’s satisfied (the lambda function returned true). If
the condition isn’t satisfied (the lambda function returned false), wait() unlocks
the mutex and puts the thread in a blocked or waiting state. When the condition vari-
able is notified by a call to notify_one() from the data-preparation thread, the thread
wakes from its slumber (unblocks it), reacquires the lock on the mutex, and checks
the condition again, returning from wait() with the mutex still locked if the condi-
tion has been satisfied. If the condition hasn’t been satisfied, the thread unlocks the
mutex and resumes waiting. This is why you need the std::unique_lock rather than
the std::lock_guard—the waiting thread must unlock the mutex while it’s waiting
and lock it again afterward, and std::lock_guard doesn’t provide that flexibility. If
the mutex remained locked while the thread was sleeping, the data-preparation
thread wouldn’t be able to lock the mutex to add an item to the queue, and the wait-
ing thread would never be able to see its condition satisfied.
*/
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

bool more_data_to_prepare()
{
    return false;
}

struct data_chunk
{};

data_chunk prepare_data()
{
    return data_chunk();
}

void process(data_chunk&)
{}

bool is_last_chunk(data_chunk&)
{
    return true;
}

std::mutex mut;
std::queue<data_chunk> data_queue;
std::condition_variable data_cond;

void data_preparation_thread()
{
	while( more_data_to_prepare() )
	{
		data_chunk const data = prepare_data();
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(data);
		data_cond.notify_one();
	}
}

void data_processing_thread()
{
	while(true)
	{
		//unique_lock as condition variable unlocks the lock if condition is not satisfied
		// Also, we need to manually unlock later
		std::unique_lock<std::mutex> lk(mut);

		// 1. Acq lock
		// 2. Check the condition( queue to become non-empty)
		// 3. if ( condition satisfied )
		//		then
		//			keep the mutex locked and proceed further line of code
		//	  else
		//			unlock the mutex and wait to get notified by other thread
		data_cond.wait(lk, [] {
				return !data_queue.empty();
		});

		data_chunk data = data_queue.front();
		data_queue.pop();

		// release lock as below code does not cause race
		lk.unlock();

		process(data);

		if( is_last_chunk(data) )
		{
			break;
		}
	}
}

int main(int argc, char **argv) {
	std::thread t1(data_preparation_thread);
	std::thread t2(data_processing_thread);
	return 0;
}



