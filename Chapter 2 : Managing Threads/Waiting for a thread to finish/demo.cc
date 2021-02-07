/*
 * demo.cc
 *
 *  Created on: 05-Feb-2021
 *      Author: prateek
 *      Pg 19
 */
#include <thread>

void doSomething(int &i)
{
	++i;
}

struct func{
	int &i;

	/*i member variable will store ref to a int which is in scope of caller */
	func(int &_i) : i(_i){}

	void operator ()(){
		for(int j = 0 ; j < 1000000 ; j++ )
		{
			//ref i mem var and perform some operation on it
			doSomething(i);
		}
	}

};

void doSomethingInCurrentThread(){}

int main(int argc, char **argv) {
	int some_local_state = 0 ;
	func my_func(some_local_state);

	std::thread t(my_func);		// launch a thread
	try{
		doSomethingInCurrentThread();
	}
	catch (...) {
		// Before returning from this thread in case of exception, wait for other launch which is referencing
		// variable some_local_state ( in this function scope) to finish. Otherwise this other other thread will
		// result in undefined behavior
		t.join();
		throw;
	}

	// Before returning from this thread , wait for other launch which is referencingv ariable some_local_state ( in this function scope) to finish.
    //Otherwise this other other thread will result in undefined behavior
	t.join();

	return 0;
}








