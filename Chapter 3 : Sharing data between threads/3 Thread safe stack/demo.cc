/*
 * demo.cc
 *
 *  Created on: 07-Feb-2021
 *      Author: prateek
 *      Pg 45
 *
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
	threadsafe_stack(){}

	threadsafe_stack(const threadsafe_stack &other)
	{
		std::lock_guard<std::mutex> lock(other.m);
		data = other.data;
	}

	//delete assignement operator
	threadsafe_stack& operator = (const threadsafe_stack &) = delete;

	void push(T new_value)
	{
		std::lock_guard<std::mutex> lock(m);
		data.push(new_value);
	}

	//pop : option 1
	std::shared_ptr<T> pop()
	{
		std::lock_guard<std::mutex> lock(m);
		if( data.empty() )
		{
			throw empty_stack();
		}
		std::shared_ptr<T> const popped_item_ptr(std::make_shared<T>(data.top()));
		data.pop();
		return popped_item_ptr;
	}

	//pop : option 2
	void pop(T &value)
	{
		std::lock_guard<std::mutex> lock(m);
		if( data.empty() )
		{
			throw empty_stack();
		}

		value = data.top();
		data.pop();
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock(m);
		return data.empty();
	}
};

int main(int argc, char **argv) {
	threadsafe_stack<int> stk;
	stk.push(5);
	stk.pop();
	if( !stk.empty() )
	{
		int x;
		stk.pop(x);
	}

	return 0;
}




