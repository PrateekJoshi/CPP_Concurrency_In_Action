/*
 * demo.cc
 *
 *  Created on: 13-Feb-2021
 *      Author: prateek
 *      Pg 127
 Operations on atomic types performed with relaxed ordering don’t participate in
synchronizes-with relationships. Operations on the same variable within a single
thread still obey happens-before relationships, but there’s almost no requirement on
ordering relative to other threads. The only requirement is that accesses to a single
atomic variable from the same thread can’t be reordered; once a given thread has
seen a particular value of an atomic variable, a subsequent read by that thread can’t
retrieve an earlier value of the variable. Without any additional synchronization, the
modification order of each variable is the only thing shared between threads that are
using memory_order_relaxed.

This time the assert f can fire, because the load of x e can read false, even though
the load of y d reads true and the store of x B happens-before the store of y c.
x and y are different variables, so there are no ordering guarantees relating to the visi-
bility of values arising from operations on each.
Relaxed operations on different variables can be freely reordered provided they
obey any happens-before relationships they’re bound by (for example, within the
same thread). They don’t introduce synchronizes-with relationships. The happens-
before relationships from listing 5.5 are shown in figure 5.4, along with a possible out-
come. Even though there’s a happens-before relationship between the stores and
between the loads, there isn’t one between either store and either load, and so the
loads can see the stores out of order.
 */
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x,y;
std::atomic<int> z;

void write_x_then_y()
{
	/* Either of the two statements can be executed first as memory ordreing is relaxed and
	 * both variabes are different on which atomic operation is performed
	 */
	x.store(true, std::memory_order_relaxed);
	y.store(true, std::memory_order_relaxed);
}

void read_y_then_x()
{
	while(!y.load(std::memory_order_relaxed) );
	if( x.load(std::memory_order_relaxed))
	{
		++z;
	}
}

int main(int argc, char **argv) {
	x = false;
	y = false;
	z = 0;

	std::thread a(write_x_then_y);
	std::thread b(read_y_then_x);

	a.join();
	b.join();

	assert(z.load() != 0 );			// can fire or cannot be fired
}



































