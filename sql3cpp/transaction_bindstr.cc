#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

Transaction &Transaction::bind(std::string const &str) {
  // Make sure that we are not already in an error state.
  check_error();
    
  int ret;
  if ( (ret = sqlite3_bind_text(stmt, ++pos, str.c_str(),
				strlen(str.c_str()), 0)) ) {
    conn.rollback_transaction();
      
    std::ostringstream os;
    os << "failed to bind string '" << str << "' to pos " << pos << ": "
       << sqlite3_errmsg(conn.connection()) << ", code " << ret;
    throw db::Exception(os.str());
  }
  return *this;
}

