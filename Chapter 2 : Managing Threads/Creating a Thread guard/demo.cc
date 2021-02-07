/*
 * demo.cc
 *
 *  Created on: 05-Feb-2021
 *      Author: prateek
 *      Pg 20
 *      When the execution of the current thread reaches the end of f, the local objects
		are destroyed in reverse order of construction. Consequently, the thread_guard
		object g is destroyed first, and the thread is joined with in the destructor c . This
		even happens if the function exits because do_something_in_current_thread throws
		an exception. The destructor of thread_guard in listing 2.3 first tests to see if the std::thread
		object is joinable() B before calling join() c. This is important, because join()
		can be called only once for a given thread of execution, so it would therefore be a mis-
		take to do so if the thread had already been joined.
 *
 */
#include <thread>

class thread_guard{
	std::thread &t;
public:
	explicit thread_guard(std::thread &_t): t(_t){}

	~thread_guard()
	{
		if( t.joinable() )
		{
			t.join();
		}
	}

	thread_guard(thread_guard const&) = delete;
	thread_guard& operator= (thread_guard const &) = delete;
};

void do_something(int &i){
	i++;
}

struct func
{
    int& i;

    func(int& i_):i(i_){}

    void operator()()
    {
        for(unsigned j=0;j<1000000;++j)
        {
            do_something(i);
        }
    }
};

void do_something_in_current_thread()
{}


int main(int argc, char **argv) {
	int some_local_state;
	func my_func(some_local_state);

	std::thread t(my_func);
	thread_guard tg(t);

	do_something_in_current_thread();

	return 0;
}



