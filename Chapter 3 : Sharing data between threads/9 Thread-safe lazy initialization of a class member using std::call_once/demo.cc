/*
 * demo.cc
 *
 *  Created on: 08-Feb-2021
 *      Author: prateek
 *      Pg 61
Rather than locking a mutex and explicitly checking the pointer, every
thread can just use std::call_once , safe in the knowledge that the pointer will have
been initialized by some thread (in a properly synchronized fashion) by the time
std::call_once returns.

In that example, the initialization is done either by the first call to send_data() B
or by the first call to receive_data() d. The use of the member function open_
connection() to initialize the data also requires that the this pointer be passed in.
Just as for other functions in the Standard Library that accept callable objects, such as
the constructor for std::thread and std::bind() , this is done by passing an addi-
tional argument to std::call_once() c.
It’s worth noting that, like std::mutex, std::once_flag instances can’t be copied
or moved, so if you use them as a class member like this, you’ll have to explicitly
define these special member functions should you require them.

 *
 */
#include <mutex>

struct connection_info{};

struct data_packet{};

struct connection_handle
{
	void send_data(data_packet const &)
	{}

	data_packet receive_data()
	{
		return data_packet();
	}
};

struct remote_connection_manager
{
	connection_handle open(connection_info const &)
	{
		return connection_handle();
	}
}connection_manager;

class X
{
private:
	connection_info connection_details;
	connection_handle connection;
	std::once_flag connection_init_flag;

	void open_connection()
	{
		connection = connection_manager.open(connection_details);
	}
public:
	X(connection_info const &connection_details_) : connection_details(connection_details_)
	{}

	void send_data(data_packet const &data)
	{
		std::call_once(connection_init_flag, &X::open_connection, this);
		connection.send_data(data);
	}

	data_packet receive_data()
	{
		std::call_once(connection_init_flag, &X::open_connection, this);
		return connection.receive_data();
	}
};

int main(int argc, char **argv) {
	return 0;
}
