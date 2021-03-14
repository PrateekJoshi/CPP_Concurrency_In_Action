/*
 * demo.cc
 *
 *  Created on: 14-Mar-2021
 *      Author: prateek
 *		Copy of : ../2 A thread pool with waitable tasks/demo.cc
 *      Pg 277
 */

#include <algorithm>
#include <atomic>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <vector>


/* Implementation in other chapter */
struct join_threads
{
    join_threads(std::vector<std::thread>&)
    {}
};

class function_wrapper
{
	struct impl_base
	{
		virtual void call() = 0;
		virtual ~impl_base() {}
	};

	std::unique_ptr<impl_base> impl;
	template<typename F>
	struct impl_type : impl_base
	{
		F f;
		impl_type(F&& f_) : f(std::move(f_)) {}
		void call()
		{
			f();
		}
	};

public:

	template<typename F>
	function_wrapper(F&& f ) : impl(new impl_type<F>(std::move(f)))
	{}

	/* Overload () operator */
	void operator () ()
	{
		impl->call();
	}

	function_wrapper() = default;

	function_wrapper(function_wrapper&& other) : impl(std::move(other.impl))
	{}

	function_wrapper& operator = (function_wrapper&& other)
	{
		impl = std::move(other.impl);
		return *this;
	}

	function_wrapper(const function_wrapper&) = delete;
	function_wrapper(function_wrapper&) = delete;
	function_wrapper& operator = (const function_wrapper&) = delete;
};

class thread_pool
{
public:
	/* Constructor */
	thread_pool() : done(false), joiner(threads)
	{
		// Get hardware concurrency count
		unsigned const thread_count = std::thread::hardware_concurrency();

		try
		{
			// Create threads
			for( unsigned i = 0 ; i < thread_count ; i++ )
			{
				threads.push_back(std::thread(&thread_pool::worker_thread, this));
			}
		}
		catch(...)
		{
			/*
			 * Thread can fail by throwing an exception, so you need to ensure that any threads yo have
			 * started are stopped and cleaned up nicely in this case. This is achieved with a try-catch
			 * block that sets the done flag when an exception is thrown.
			 */
			done = true;
			throw;
		}
	}

	/* Destructor */
	~thread_pool()
	{
		done = true;
	}

	/* Submit a task to work queue
	 *1. Modified submit() function to return a std::future<> to hold the return value of the task
	 *   and allow the caller to wait for the task to complete.
	 *2. This requires that you know the return type of the supplied function f, which is where
	 *   std::result_of<> comes in.
	 *   std::result_of<FunctionType()>::type is the type of the result of invoking an instance of type
	 *   FunctionType (such as f) with no arguments.
	 *3. You can use same std::result_of<> expression for the result_type typedef inside the function.
	 */
	template<typename FunctionType>
	std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f)
	{
		//You can use same std::result_of<> expression for the result_type typedef inside the function.
		typedef typename std::result_of<FunctionType()>::type result_type;

		/* Wrap the function f in a std::packaged_task<result_type()>, because f is a function or callable
		 * object that takes no parameters and returns an instance of type result_type.
		 */
		std::packaged_task<result_type()> task(std::move(f));

		/* You can now get your future from the std::packaged_task<>, before pushing the task onto the queue
		 * and returning the future
		 * NOTE : Use std::move() when pushing the task onto the queue, because std::packaged_task<> is'nt
		 * copyable.
		 * The queue now stores function_wrapper objects rather than std::function<void()> objects in order to
		 * handle this.
		 */
		std::future<result_type> result(task.get_future());
		work_queue.push(std::move(task));
		return result;
	}

	/*
	 * Pop a pending task from work queue and run ( for manual management on work queue)
	 */
	void thread_pool::run_pending_task()
	{
		function_wrapper task;
		if( work_queue.try_pop(task) )
		{
			task();
		}
		else
		{
			std::this_thread::yield();
		}
	}

private:
	void worker_thread()
	{
		while( !done )
		{
			function_wrapper task;
			if( work_queue.try_pop(task) )
			{
				task();
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

	std::atomic_bool done;
	/* NOTE : Used function_wrapper rather than std::function */
	thread_safe_queue<function_wrapper> work_queue;
	std::vector<std::thread> threads;
	join_threads joiner;
};

