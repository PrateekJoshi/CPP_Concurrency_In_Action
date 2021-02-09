/*
 * demo.cc
 *
 *  Created on: 09-Feb-2021
 *      Author: prateek
 *      Pg 80
 Many GUI frameworks require that updates to the GUI be done from specific threads,
so if another thread needs to update the GUI, it must send a message to the right
thread in order to do so. std:packaged_task provides one way of doing this without
requiring a custom message for each and every GUI-related activity, as shown here.

This code is very simple: the GUI thread B loops until a message has been received
telling the GUI to shut down c, repeatedly polling for GUI messages to handle d,
such as user clicks, and for tasks on the task queue. If there are no tasks on the queue e,
it loops again; otherwise, it extracts the task from the queue f, releases the lock on
the queue, and then runs the task g. The future associated with the task will then be
made ready when the task completes.

Posting a task on the queue is equally simple: a new packaged task is created from
the supplied function h, the future is obtained from that task i by calling the
get_future() member function, and the task is put on the list j before the future is
returned to the caller 1). The code that posted the message to the GUI thread can then
wait for the future if it needs to know that the task has been completed, or it can discard
the future if it doesn’t need to know.

This example uses std::packaged_task<void()> for the tasks, which wraps a
function or other callable object that takes no parameters and returns void (if it
returns anything else, the return value is discarded). This is the simplest possible task,
but as you saw earlier, std::packaged_task can also be used in more complex situa-
tions—by specifying a different function signature as the template parameter, you can
change the return type (and thus the type of data stored in the future’s associated state)
and also the argument types of the function call operator. This example could easily be
extended to allow for tasks that are to be run on the GUI thread to accept arguments
and return a value in the std::future rather than just a completion indicator.
*/
#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

std::mutex mu;
std::deque<std::packaged_task<void()>> tasks;

bool gui_shutdown_message_received();
void get_and_process_gui_message();

void gui_thread()
{
	while( !gui_shutdown_message_received() )
	{
		get_and_process_gui_message();

		std::packaged_task<void()> task;
		{
			std::lock_guard<std::mutex> lk(mu);
			if( tasks.empty() )
			{
				continue;
			}

			task = std::move(tasks.front());
			tasks.pop_front();
		}

		task();
	}
}

std::thread gui_bg_thread(gui_thread);

template<typename Func>
std::future<void> post_task_for_gui_Thread(Func f)
{
	std::packaged_task<void()> task(f);
	std::future<void> result = task.get_future();

	std::lock_guard<std::mutex> lk(mu);
	tasks.push_back(std::move(task));
	return result;
}





















