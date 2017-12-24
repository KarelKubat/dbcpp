#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

Transaction &Transaction::bind(std::string const &str) {
  // Make sure that we are not already in an error state.
  check_error();

  int ret;
  if ( (ret = sqlite3_bind_text(stmt, ++pos, str.c_str(), str.length(), 0)) ) {
    conn.rollback_transaction();

    std::ostringstream os;
    os << "failed to bind string '" << str << "' to pos " << pos << ": "
       << sqlite3_errstr(ret) << ", code " << ret
       << ", sql " << last_sql;
    throw db::Exception(os.str());
  }
  return *this;
}
