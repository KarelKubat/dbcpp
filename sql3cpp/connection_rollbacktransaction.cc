#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::rollback_transaction() {
  check_connected();
  if (in_transaction_map.count(db) > 0 && in_transaction_map[db]) {
    exec_sql("ROLLBACK TRANSACTION");
    in_transaction_map[db] = false;
  }
}
