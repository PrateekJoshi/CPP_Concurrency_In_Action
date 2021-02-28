/*
 * demo.cc
 *
 *  Created on: 28-Feb-2021
 *      Author: prateek
 *      Pg 166
 */
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class threadsafe_queue
{
private:
	struct node
	{
		std::shared_ptr<T> data;
		std::unique_ptr<node> next;
	};

	std::mutex head_mutex;
	std::unique_ptr<node> head;
	std::mutex tail_mutex;
	node* tail;
	std::condition_variable data_cond;

	node* get_tail();
	std::unique_ptr<node> pop_head();
	std::unique_lock<std::mutex> wait_for_data();
	std::unique_ptr<node> wait_pop_head();
	std::unique_ptr<node> wait_pop_head(T& value);
	std::unique_ptr<node> try_pop_head();
	std::unique_ptr<node> try_pop_head(T& value);


public:
	threadsafe_queue() : head(new node), tail(head.get())
	{}

	threadsafe_queue(const threadsafe_queue& other) = delete;
	threadsafe_queue& operator= (const threadsafe_queue &other) = delete;

	std::shared_ptr<T> try_pop();
	bool try_pop(T& value);
	std::shared_ptr<T> wait_and_pop();
	void wait_and_pop(T& value);
	void push(T new_value);
	bool empty();

};

/*
 * Push new_value of type T into the queue
 */
template<typename T>
void threadsafe_queue<T>::push(T new_value)
{
	std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
	std::unique_ptr<node> p(new node);
	{
		std::lock_guard<std::mutex> tail_lock(tail_mutex);
		tail->data = new_data;
		node* const new_tail = p.get();
		tail->next = std::move(p);
		tail = new_tail;
	}

	data_cond.notify_one();
}

/*
 * Get tail pointer of the queue
 */
template<typename T>
threadsafe_queue::node* threadsafe_queue<T>::get_tail()
{
	std::lock_guard<std::mutex> tail_lock(tail_mutex);
	return tail;
}

/*
 * Pop head of the queue and return pointer to popped head node
 */
template<typename T>
std::unique_ptr<threadsafe_queue::node> threadsafe_queue<T>::pop_head()
{
	std::unique_ptr<threadsafe_queue::node> const old_head = std::move(head);
	head = std::move(old_head->next);
	return old_head;
}

/*
 * Wait for queue to become non-empty and then return head_lock
 */
template<typename T>
std::unique_lock<std::mutex> threadsafe_queue<T>::wait_for_data()
{
	std::unique_lock<std::mutex> head_lock(head_mutex);
	data_cond.wait(head_lock, [&] { return head != get_tail() ;});
	return std::move(head_lock);
}

/*
 * Pop the head from queue and return popped head
 */
template<typename T>
std::unique_ptr<threadsafe_queue::node> threadsafe_queue<T>::wait_pop_head()
{
	/* Cond wait for queue to be non-empty and then get the lock of head node */
	std::unique_lock<std::mutex> head_lock(wait_for_data());
	return pop_head();	// return popped head
 }

/*
 * Pop the head from queue and return popped head with value of popped head in parameter value
 */
template<typename T>
std::unique_ptr<threadsafe_queue::node> threadsafe_queue<T>::wait_pop_head(T& value)
{
	/* Cond wait for queue to be non-empty and then get the lock of head node */
	std::unique_lock<std::mutex> head_lock(wait_for_data());

	/* Store value of popped head */
	value = std::move(*head->data);
	return pop_head(); // return popped head
}

/*
 * Wait for queue to be non-empty and then return pointer to popped data of type T in node
 */
template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::wait_and_pop()
{
	std::unique_ptr<threadsafe_queue::node> const old_head = wait_pop_head();
	return old_head->data;
}

/*
 * Wait for queue to be non-empty and then return popped data in value
 */
template<typename T>
void threadsafe_queue<T>::wait_and_pop(T& value)
{
	std::unique_ptr<threadsafe_queue::node> const old_head = wait_pop_head(value);
}

/*
 * Non waiting pop head , return nullptr if queue is empty
 */
template<typename T>
std::unique_ptr<threadsafe_queue::node> threadsafe_queue<T>::try_pop_head()
{
	std::lock_guard<std::mutex> head_lock(head_mutex);
	if( head.get() == get_tail() )
	{
		return std::unique_ptr<threadsafe_queue::node>();
	}

	return pop_head();
}

/*
 * Non waiting pop head, return popped value of type T in value parameter and return pointer to node
 * of popped
 */
template<typename T>
std::unique_ptr<threadsafe_queue::node> threadsafe_queue<T>::try_pop_head(T& value)
{
	std::lock_guard<std::mutex> head_lock(head_mutex);
	if( head.get() == get_tail() )
	{
		return std::unique_ptr<threadsafe_queue::node>();
	}

	value = std::move(*head->data);
	return pop_head();
}

/*
 * Non waiting pop. Return pointer to T data inside node
 */
template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::try_pop()
{
	std::unique_ptr<threadsafe_queue::node> const old_head = try_pop_head();
	return old_head ? old_head->data : std::shared_ptr<T>();
}

/*
 * Non waiting pop. Return pointer to T data in value argument and true of popped and false
 * if queue is empty
 */
template<typename T>
bool threadsafe_queue<T>::try_pop(T& value)
{
	std::unique_ptr<threadsafe_queue::node> const old_head = try_pop_head(value);
	return old_head;
}

/*
 * Check if queue is empty
 */
template<typename T>
bool threadsafe_queue<T>::empty()
{
	std::lock_guard<std::mutex> head_lock(head_mutex);
	return ( head == get_tail());
}

