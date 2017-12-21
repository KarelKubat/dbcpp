#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::begin_transaction() {
  check_connected();

  // Issue BEGIN TRANSACTION only when the class' static map of the
  // connection says that we're not in a transaction yet. Re-beginning
  // a transaction would be a database error.
  // For threaded applications, make sure that this is protected by a
  // mutex.
  if (! in_transaction_map.count(db) || !in_transaction_map[db]) {
    trx_map_change_mutex.lock();
    try {
      exec_sql("BEGIN TRANSACTION");
      in_transaction_map[db] = true;
      trx_map_change_mutex.unlock();
    } catch (std::exception const &e) {
      trx_map_change_mutex.unlock();
      throw e;
    }
  }
}
