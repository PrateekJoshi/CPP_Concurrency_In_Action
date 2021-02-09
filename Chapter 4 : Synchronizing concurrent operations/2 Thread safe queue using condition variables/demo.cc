/*
 * demo.cc
 *
 *  Created on: 08-Feb-2021
 *      Author: prateek
 *      Pg 74
When using a queue to pass data between threads, the receiving thread often needs to wait for the
data. Let’s provide two variants on pop(): try_pop(), which tries to pop the value
from the queue but always returns immediately (with an indication of failure) even if
there wasn’t a value to retrieve, and wait_and_pop() , which will wait until there’s a
value to retrieve. If you take your lead for the signatures from the stack example, your
interface looks like the following.
 *
 */
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

template<typename T>
class threadsafe_queue
{
private:
	mutable std::mutex mut;
	std::queue<T> data_queue;
	std::condition_variable data_cond;
public:
	threadsafe_queue(){}

	threadsafe_queue(threadsafe_queue const &other)
	{
		std::lock_guard<std::mutex> lk(other.mut);
		data_queue = other.data_queue;
	}

	void push(T new_value)
	{
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(new_value);
		data_cond.notify_one();
	}

	void wait_and_pop(T &value)
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk,[this]{
			return !data_queue.empty();
		});

		value = data_queue.front();
		data_queue.pop();
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this] {
			return !data_queue.empty();
		});

		std::shared_ptr<T> result(std::make_shared<T>(data_queue.front()));
		data_queue.pop();
		return result;
	}

	bool try_pop( T &value )
	{
		std::lock_guard<std::mutex> lk(mut);
		if( data_queue.empty() )
		{
			return false;
		}
		value = data_queue.front();
		data_queue.pop();
		return true;
	}

	std::shared_ptr<T> try_pop()
	{
		std::lock_guard<std::mutex> lk(mut);
		if( data_queue.empty() )
		{
			return std::shared_ptr<T>();
		}
		std::shared_ptr<T> result(std::make_shared(data_queue.front));
		data_queue.pop();
		return result;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.empty();
	}
};

int main(int argc, char **argv) {
	return 0;
}



