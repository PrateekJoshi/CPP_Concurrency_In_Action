/*
 * demo.cc
 *
 *  Created on: 14-Feb-2021
 *      Author: prateek
 *      Pg 139
One important use for this kind of memory ordering is where the atomic opera-
tion loads a pointer to some data. By using memory_order_consume on the load and
memory_order_release on the prior store, you ensure that the pointed-to data is cor-
rectly synchronized, without imposing any synchronization requirements on any other
nondependent data. The following listing shows an example of this scenario.


Even though the store to a B is sequenced before the store to p c, and the store
to p is tagged memory_order_release, the load of p d is tagged memory_order_
consume. This means that the store to p only happens-before those expressions that
are dependent on the value loaded from p. This means that the asserts on the data
members of the X structure e, f are guaranteed not to fire, because the load of p
carries a dependency to those expressions through the variable x. On the other
hand, the assert on the value of a g may or may not fire; this operation isn’t
dependent on the value loaded from p, and so there’s no guarantee on the value
that’s read. This is particularly apparent because it’s tagged with memory_order_
relaxed, as you’ll see.
 */
#include <string>
#include <thread>
#include <atomic>
#include <assert.h>

struct X
{
	int i;
	std::string s;
};

std::atomic<X*> p;
std::atomic<int> a;

void create_x()
{
	X *x = new X;
	x->i = 42;
	x->s = "hello";

	a.store(99, std::memory_order_relaxed);
	p.store(x, std::memory_order_release);
}

void use_x()
{
	X *x;
	while( !( x = p.load(std::memory_order_consume)))
	{
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}

	assert(x->i == 42);
	assert(x->s == "hello");
	assert(a.load(std::memory_order_relaxed) == 99 );
}

int main(int argc, char **argv) {
	std::thread t1(create_x);
	std::thread t2(use_x);

	t1.join();
	t2.join();
}



