/*
 * demo.cc
 *
 *  Created on: 08-Feb-2021
 *      Author: prateek
Using a std::mutex to protect the data structure is therefore overly pessi-
mistic, because it will eliminate the possible concurrency in reading the data structure
when it isn’t undergoing modification; what’s needed is a different kind of mutex.
This new kind of mutex is typically called a reader-writer mutex, because it allows for
two different kinds of usage: exclusive access by a single “writer” thread or shared,
concurrent access by multiple “reader” threads.

Those threads that don’t need to update the data structure can instead use boost::shared_lock<boost::shared_mutex>
to obtain shared access. This is used just the same as std::unique_lock, except that
multiple threads may have a shared lock on the same boost::shared_mutex at the
same time. The only constraint is that if any thread has a shared lock, a thread that
tries to acquire an exclusive lock will block until all other threads have relinquished
their locks, and likewise if any thread has an exclusive lock, no other thread may
acquire a shared or exclusive lock until the first thread has relinquished its lock.

In listing 3.13, find_entry() uses an instance of boost::shared_lock<> to protect it
for shared, read-only access B; multiple threads can therefore call find_entry()
simultaneously without problems. On the other hand, update_or_add_entry() uses
an instance of std::lock_guard<> to provide exclusive access while the table is
updated c; not only are other threads prevented from doing updates in a call update_
or_add_entry(), but threads that call find_entry() are blocked too.

  NOTE : C++17 has shared_mutex capability.
 */
#include <map>
#include <string>
#include <mutex>
#include <boost/thread/shared_mutex.hpp>

class dns_entry{};

class dns_cache
{
	std::map<std::string,dns_entry> entries;
	mutable boost::shared_mutex entry_mutex;
public:
	dns_entry find_entry(std::string const &domain) const
	{
		boost::shared_lock<boost::shared_mutex> lock(entry_mutex);
		std::map<std::string, dns_entry>::const_iterator const itr = entries.find(domain);
		return ( itr == entries.end() ) ? dns_entry() : itr->second;
	}

	void update_or_add_entry(std::string const &domain, dns_entry const &dns_details)
	{
		std::lock_guard<boost::shared_mutex> lock(entry_mutex);
		entries[domain] = dns_details;
	}
};

int main(int argc, char **argv) {
	return 0;
}


