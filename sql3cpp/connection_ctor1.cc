#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

std::map<sqlite3*, int>  Connection::usage_map;
std::map<sqlite3*, bool> Connection::in_transaction_map;
std::mutex               Connection::trx_map_change_mutex;

Connection::Connection():
  db(0) {
}
