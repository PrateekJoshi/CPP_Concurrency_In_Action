/*
 * demo.cc
 *
 *  Created on: 07-Feb-2021
 *      Author: prateek
 *      Pg 48
First, the arguments are checked to ensure they are different instances, because
attempting to acquire a lock on a std::mutex when you already hold it is undefined
behavior. (A mutex that does permit multiple locks by the same thread is provided in
the form of std::recursive_mutex. See section 3.3.3 for details.) Then, the call to
std::lock() B locks the two mutexes, and two std::lock_guard instances are con-
structed c , d, one for each mutex. The std::adopt_lock parameter is supplied in
addition to the mutex to indicate to the std::lock_guard objects that the mutexes
are already locked, and they should just adopt the ownership of the existing lock on
the mutex rather than attempt to lock the mutex in the constructor.
This ensures that the mutexes are correctly unlocked on function exit in the gen-
eral case where the protected operation might throw an exception; it also allows for a
simple return.
 *
 */
#include <mutex>

class some_big_object{};

void swap(some_big_object &lhs, some_big_object &rhs){}

class X
{
private:
	some_big_object some_detail;
	mutable std::mutex m;
public:
	X(some_big_object const &sd) : some_detail(sd) {}

	friend void swap(X &lhs, X &rhs)
	{
		if( &lhs == &rhs )
		{
			//can't acquire lock more than once in the same instance ( unbdefined behavior)
			return;
		}

		std::lock(lhs.m, rhs.m);

		// adopt the ownership of the existing lock with lock_guard
		std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock);
		std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock);

		// do the operation
		swap(lhs.some_detail, rhs.some_detail);
	}
};

int main(int argc, char **argv) {
	return 0;
}





