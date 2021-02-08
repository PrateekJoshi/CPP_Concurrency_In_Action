/*
 * demo.cc
 *
 *  Created on: 08-Feb-2021
 *      Author: prateek
 *      Pg 54
std::unique_lock provides a bit more flexibility than std::lock_guard by relaxing
the invariants; a std::unique_lock instance doesn’t always own the mutex that it’s
associated with. First off, just as you can pass std::adopt_lock as a second argument
to the constructor to have the lock object manage the lock on a mutex, you can also
pass std::defer_lock as the second argument to indicate that the mutex should
remain unlocked on construction. The lock can then be acquired later by calling
lock() on the std::unique_lock object (not the mutex) or by passing the std::
unique_lock object itself to std::lock(). Listing 3.6 could just as easily have been
written as shown in listing 3.9, using std::unique_lock and std::defer_lock B
rather than std::lock_guard and std::adopt_lock . The code has the same line
count and is essentially equivalent, apart from one small thing: std::unique_lock
takes more space and is a fraction slower to use than std::lock_guard. The flexibility
of allowing a std::unique_lock instance not to own the mutex comes at a price: this
information has to be stored, and it has to be updated.

In listing 3.9, the std::unique_lock objects could be passed to std::lock() c because
std::unique_lock provides lock(), try_lock(), and unlock() member functions.
These forward to the member functions of the same name on the underlying mutex
to do the actual work and just update a flag inside the std::unique_lock instance to
indicate whether the mutex is currently owned by that instance. This flag is necessary
in order to ensure that unlock() is called correctly in the destructor. If the instance
does own the mutex, the destructor must call unlock(), and if the instance does not own
the mutex, it must not call unlock() . This flag can be queried by calling the owns_lock()
member function.
 *
 */
#include <mutex>

class some_big_object{};

void swap(some_big_object &lhs, some_big_object &rhs)
{

}

class X
{
private:
	some_big_object some_detail;
	mutable std::mutex m;

public:
	X(some_big_object const &sd) : some_detail(sd){}

	friend void swap(X &lhs , X &rhs)
	{
		if( &lhs == &rhs )
		{
			return;
		}

		/* std::defer_lock leaves mutexes unlocked */
		std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock);
		std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock);

		/* Mutexes are locked here */
		std::lock(lock_a, lock_b);

		swap(lhs.some_detail, rhs.some_detail);
	}
};

int main(int argc, char **argv) {
	return 0;
}


