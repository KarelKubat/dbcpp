#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

std::map<sqlite3*, int>  Connection::usage_map;
std::mutex               Connection::connection_mutex;

Connection::Connection():
  db(0) {
}
