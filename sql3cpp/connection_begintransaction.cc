#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::begin_transaction() {
  check_connected();
  if (! in_transaction_map.count(db) || !in_transaction_map[db]) {
    exec_sql("BEGIN TRANSACTION");
    in_transaction_map[db] = true;
  }
}
