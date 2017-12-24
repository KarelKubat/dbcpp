#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

std::map<sqlite3*, int> *Connection::refcount;
std::mutex              Connection::refmutex;

Connection::Connection():
  db(0) {
}
