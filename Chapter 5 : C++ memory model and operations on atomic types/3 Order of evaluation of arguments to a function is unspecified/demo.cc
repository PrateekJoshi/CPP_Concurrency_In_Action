/*
 * demo.cc
 *
 *  Created on: 13-Feb-2021
 *      Author: prateek
 *      Pg 122
You know that the program in the following listing
will output “1,2” or “2,1”, but it’s unspecified which, because the order of the two
calls to get_num()is unspecified.

There are circumstances where operations within a single statement are sequenced
such as where the built-in comma operator is used or where the result of one expres-
sion is used as an argument to another expression. But in general, operations within a
single statement are nonsequenced, and there’s no sequenced-before (and thus no
happens-before) relationship between them. Of course, all operations in a statement
happen before all of the operations in the next statement.
 */
#include <iostream>

void foo(int a, int b)
{
	std::cout<<a<<", "<<b<<std::endl;
}

int get_num()
{
	static int i = 0;
	return ++i;
}

int main(int argc, char **argv) {
	foo(get_num(), get_num());			// call to get_num() are unordered
}


