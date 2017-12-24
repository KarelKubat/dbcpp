#include <iostream>
#include <thread>

#include <sql3cpp.h>

using namespace sql3;

bool Connection::decrefcount() const {
    bool ret = false;

    if (!db|| !refcount)
	return false;

    /*
     * This is unexpectedly ugly, we must ignore exceptions raised by locking
     * a mutex.
     * Reason is that a global sql3::Connection object gets destroyed when there
     * is no more a running thread, which makes locking impossible.
     *
     * Hints how to make this elegant will are appreciated :P
     */
    try {
	refmutex.lock();
    } catch (std::exception e) {
	return false;
    }
    
    if (refcount->count(db)) {
	--refcount->at(db);
	ret = refcount->at(db) == 0;
    }
    refmutex.unlock();

    #ifdef DEBUG
    std::cout << "connection " << db << " decremented to "
	      << refcount->at(db) << '\n';
    #endif
    
    return ret;
}
