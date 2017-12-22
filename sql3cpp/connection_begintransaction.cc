#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::begin_transaction() {
  check_connected();
  exec_sql("BEGIN TRANSACTION");
}
