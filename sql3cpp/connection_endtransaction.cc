#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::end_transaction() {
  check_connected();
  if (in_transaction_map.count(db) > 0 && in_transaction_map[db]) {
    trx_map_change_mutex.lock();
    try {
      exec_sql("END TRANSACTION");
      in_transaction_map[db] = false;
      trx_map_change_mutex.unlock();
    } catch (std::exception const &e) {
      trx_map_change_mutex.unlock();
      throw e;
    }
  }
}
