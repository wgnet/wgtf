#ifndef WG_READ_WRITE_LOCK
#define WG_READ_WRITE_LOCK

#include <mutex>

#include "wg_condition_variable.hpp"

namespace wgt
{
/// Read / write lock implementation with mutex and conditional variable
class wg_read_write_lock
{
public:

	wg_read_write_lock();
	~wg_read_write_lock();

	void read_lock();
	void read_unlock();
	void write_lock();
	void write_unlock();

private:

	bool writer_;
	int readers_;
	mutable std::mutex mutex_;
	wg_condition_variable unlocked_;
};

/// Read lock guard
class wg_read_lock_guard
{
public:
	explicit wg_read_lock_guard( wg_read_write_lock & lock ) : read_Lock_( lock ) { read_Lock_.read_lock(); }
	~wg_read_lock_guard() { read_Lock_.read_unlock(); }

private:
	wg_read_write_lock & read_Lock_;
};

/// Write lock guard
class wg_write_lock_guard
{
public:
	explicit wg_write_lock_guard( wg_read_write_lock & lock ) : write_Lock_( lock ) { write_Lock_.write_lock(); }
	~wg_write_lock_guard() { write_Lock_.write_unlock(); }

private:
	wg_read_write_lock & write_Lock_;
};
} // end namespace wgt
#endif // WG_READ_WRITE_LOCK
