#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

sqlite3 *Connection::connection() {
  return db;
}
