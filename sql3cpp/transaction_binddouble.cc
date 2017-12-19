#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

Transaction &Transaction::bind(double val) {
  // Make sure that we are not already in an error state.
  check_error();
    
  // sqlite3 treats pos as 1-based instead of 0-based, uggh
  int ret;
  if ( (ret = sqlite3_bind_double(stmt, ++pos, val)) ) {
    conn.rollback_transaction();
      
    std::ostringstream os;
    os << "failed to bind int " << val << " to pos " << pos << ": "
       << sqlite3_errmsg(conn.connection()) << ", code " << ret;
    throw db::Exception(os.str());
  }
  return *this;
}
