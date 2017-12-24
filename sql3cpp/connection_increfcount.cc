#include <iostream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::increfcount() const {
    if (!db)
	return;
    
    refmutex.lock();

    if (!refcount)
	refcount = new std::map<sqlite3*, int>;
    
    if (! refcount->count(db))
	refcount->emplace(db, 1);
    else
	++refcount->at(db);
    
    refmutex.unlock();

    #ifdef DEBUG
    std::cout << "connection " << db << " incremented to "
	      << refcount->at(db) << '\n';
    #endif
}

