/*
 * demo.cc
 *
 *  Created on: 09-Feb-2021
 *      Author: prateek
 *      Pg 77
 */
#include <string>
#include <future>

struct X
{
	void foo(int, std::string const &);
	std::string bar(std::string const &);
};

X x;

auto f1 = std::async(&X::foo, &x, 42, "hello");		// Calls p->foo(42, "hello") , where p is &x

auto f2 = std::async(&X::bar, x, "goodbye");		// Call tmpx.bar("goodbye) where tmpx is a copy of x

struct Y
{
	double operator() (double);
};

Y y;
auto f3 = std::async(Y(), 3.141);					// Calls tmpy(3.141) where tmpy is move constructed from Y()
auto f4 = std::async(std::ref(y), 2.718);			// Calls y(2.718)

X baz(X&);

auto f6 = std::async(baz, std::ref(x));						// Call baz(x)

class move_only
{
public:
	move_only();
	move_only(move_only&&)
	move_only(move_only const&) = delete;
	move_only& operator=(move_only&&);
	move_only& operator=(move_only const&) = delete;
	void operator()();
};

auto f5 = std::async(move_only());					// Calls tmp() where tmp is constructed from std::move(move_only())




