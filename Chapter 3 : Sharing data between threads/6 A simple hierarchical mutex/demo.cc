/*
 * demo.cc
 *
 *  Created on: 07-Feb-2021
 *      Author: prateek
 *      Pg 52
 *
The key here is the use of the thread_local value representing the hierarchy value
for the current thread: this_thread_hierarchy_value B. It’s initialized to the maxi-
mum value i, so initially any mutex can be locked. Because it’s declared thread_local,
every thread has its own copy, so the state of the variable in one thread is entirely inde-
pendent of the state of the variable when read from another thread. See appendix A,
section A.8, for more information about thread_local.

So, the first time a thread locks an instance of hierarchical_mutex the value of
this_thread_hierarchy_value is ULONG_MAX. By its very nature, this is greater than
any other value, so the check in check_for_hierarchy_violation() c passes. With
that check out of the way, lock()delegates to the internal mutex for the actual lock-
ing e. Once this lock has succeeded, you can update the hierarchy value f .
If you now lock another hierarchical_mutex while holding the lock on this first
one, the value of this_thread_hierarchy_value reflects the hierarchy value of the
first mutex. The hierarchy value of this second mutex must now be less than that of
the mutex already held in order for the check c to pass.

Now, it’s important to save the previous value of the hierarchy value for the current
thread so you can restore it in unlock() g; otherwise you’d never be able to lock a
mutex with a higher hierarchy value again, even if the thread didn’t hold any locks.
Because you store this previous hierarchy value only when you hold the internal_
mutex d, and you restore it before you unlock the internal mutex g, you can safely
store it in the hierarchical_mutex itself, because it’s safely protected by the lock on
the internal mutex.

try_lock() works the same as lock() except that if the call to try_lock() on the
internal_mutex fails h, then you don’t own the lock, so you don’t update the hierar-
chy value and return false rather than true.
 *
 */
#include <mutex>
#include <stdexcept>
#include <climits>

class hierarchical_mutex
{
	std::mutex internal_mutex;
	unsigned long const hierarchical_value;
	unsigned long prev_hierarchical_value;
	static thread_local unsigned long this_thread_hierarchical_value;

	void check_for_hierarchical_violation()
	{
		if( this_thread_hierarchical_value <= hierarchical_value )
		{
			throw std::logic_error("mutex hierarchy violated");
		}
	}

	void update_hierarchy_value()
	{
		prev_hierarchical_value = this_thread_hierarchical_value;
		this_thread_hierarchical_value = hierarchical_value;
	}

public:
	explicit hierarchical_mutex(unsigned long value): hierarchical_value(value), prev_hierarchical_value(0)
	{}

	void lock()
	{
		check_for_hierarchical_violation();
		internal_mutex.lock();
		update_hierarchy_value();
	}

	void unlock()
	{
		this_thread_hierarchical_value = prev_hierarchical_value;
		internal_mutex.unlock();
	}

	bool try_lock()
	{
		check_for_hierarchical_violation();
		if( !internal_mutex.try_lock() )
		{
			return false;
		}
		update_hierarchy_value();
		return true;
	}
};

/*Because it’s declared thread_local, every thread has its own copy, so the state of the variable in one thread
 is entirely independent of the state of the variable when read from another thread.
*/
thread_local unsigned long hierarchical_mutex::this_thread_hierarchical_value(ULONG_MAX);

int main(int argc, char **argv) {
	hierarchical_mutex m1(42);
	hierarchical_mutex m2(2000);
}





