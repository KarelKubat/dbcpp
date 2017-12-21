#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::exec_sql(std::string const &sql) {
  check_connected();

  char *errmsg;
  if (sqlite3_exec(db, sql.c_str(), 0, 0, &errmsg)) {
    std::ostringstream os;
    os << sql << " failed: " << errmsg;
    throw db::Exception(os.str());
  }
}
