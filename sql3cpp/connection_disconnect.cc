#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::disconnect() {
  sqlite3_close(db);
  db = 0;
}
