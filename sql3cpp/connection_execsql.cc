#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

int Connection::exec_sql(std::string const &sql) {
  int ret;
  sqlite3_stmt *stmt;

  check_connected();
  if ( (ret = sqlite3_prepare(connection(), sql.c_str(), sql.length(),
                              &stmt, 0)) )
    return ret;
  return sqlite3_step(stmt);
}
