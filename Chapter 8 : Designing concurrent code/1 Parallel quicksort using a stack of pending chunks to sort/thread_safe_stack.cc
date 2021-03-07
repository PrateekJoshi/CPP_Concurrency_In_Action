/*
 * demo.cc
 *
 *  Created on: 21-Feb-2021
 *      Author: prateek
 *      Pg 151
 */
#include <exception>
#include <stack>
#include <mutex>
#include <memory>

struct empty_stack : std::exception
{
	const char* what() const throw()
	{
		return "empty stack";
	}
};

template<typename T>
class threadsafe_stack
{
private:
	std::stack<T> data;
	mutable std::mutex m;

public:
	threadsafe_stack() {}

	//Move copy constructor
	threadsafe_stack(const threadsafe_stack &other)
	{
		std::lock_guard<std::mutex> lock(other.m);
		data = other.data;
	}

	//Delete assignment operator
	threadsafe_stack& operator = (const threadsafe_stack&) = delete;

	void push(T new_value)
	{
		std::lock_guard<std::mutex> lock(m);
		data.push(std::move(new_value));	// move R value to stack push mem func
	}

	std::shared_ptr<T> pop()
	{
		std::lock_guard<std::mutex> lock(m);
		if( data.empty() )
		{
			throw empty_stack();
		}

		// Create a shared ptr of popped item using move ( without creating a copy and getting a pointer of it )
		std::shared_ptr<T> const popped(
				std::make_shared<T>(std::move(data.top())));

		data.pop();

		return popped;
	}

	void pop(T &value)
	{
		std::lock_guard<std::mutex> lock(m);
		if( data.empty() )
		{
			throw empty_stack();
		}
		value = std::move(data.top());
		data.pop();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(m);
		return data.empty();
	}

};




