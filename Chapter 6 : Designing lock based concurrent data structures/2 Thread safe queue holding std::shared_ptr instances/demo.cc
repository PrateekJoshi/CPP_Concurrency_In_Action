/*
 * demo.cc
 *
 *  Created on: 22-Feb-2021
 *      Author: prateek
 *      Pg 156
 The basic consequences of holding the data by std::shared_ptr<> are straightfor-
ward: the pop functions that take a reference to a variable to receive the new value
now have to dereference the stored pointer B, c , and the pop functions that return
a std::shared_ptr<> instance can just retrieve it from the queue d, e before
returning it to the caller.

If the data is held by std::shared_ptr<>, there’s an additional benefit: the alloca-
tion of the new instance can now be done outside the lock in push() f, whereas in
listing 6.2 it had to be done while holding the lock in pop(). Because memory alloca-
tion is typically quite an expensive operation, this can be very beneficial for the per-
formance of the queue, because it reduces the time the mutex is held, allowing other
threads to perform operations on the queue in the meantime.

Just like in the stack example, the use of a mutex to protect the entire data structure
limits the concurrency supported by this queue; although multiple threads might be
blocked on the queue in various member functions, only one thread can be doing any
work at a time. However, part of this restriction comes from the use of std::queue<> in
the implementation; by using the standard container you now have essentially one
data item that’s either protected or not. By taking control of the detailed implementa-
tion of the data structure, you can provide more fine-grained locking and thus allow a
higher level of concurrency.
*/
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

template<typename T>
class threadsafe_queue
{
private:
	mutable std::mutex mut;
	std::queue<std::shared_ptr<T>> data_queue;
	std::condition_variable data_cond;
public:
	threadsafe_queue() {}

	void wait_and_pop(T &value)
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this]{ return !data_queue.empty() ; });
		value = std::move(*data_queue.front());
		data_queue.pop();
	}

	bool try_pop(T &value)
	{
		std::lock_guard<std::mutex> lk(mut);
		if( data_queue.empty() )
		{
			return false;
		}
		value = std::move(*data_queue.front());
		data_queue.pop();
		return true;
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(mut);
		data_cond.wait(lk, [this]{ return !data_queue.empty();});

		std::shared_ptr<T> popped = data_queue.front();
		data_queue.pop();
		return popped;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mut);
		return data_queue.empty();
	}

	void push(T new_value)
	{
		std::shared_ptr<T> data(std::make_shared(std::move(new_value)));

		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(data);
		data_cond.notify_one();
	}

};




