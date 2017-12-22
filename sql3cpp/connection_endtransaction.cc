#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::end_transaction() {
  check_connected();
  exec_sql("END TRANSACTION");
}
