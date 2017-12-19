#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

std::map<sqlite3*, int>  Connection::usage_map;
std::map<sqlite3*, bool> Connection::in_transaction_map;

Connection::Connection():
  db(0) {
}
