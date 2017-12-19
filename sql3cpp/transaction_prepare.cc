#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

Transaction &Transaction::prepare(std::string const &sql) {
  // Make sure that we are not already in an error state.
  check_error();

  // Start transaction if not done yet.
  conn.begin_transaction();

  if ( (sqlite3_prepare_v2(conn.connection(),
			   sql.c_str(),
			   strlen(sql.c_str()),
			   &stmt,
			   0)) ) {
    conn.rollback_transaction();

    std::ostringstream os;
    os << "failed to prepare '" << sql << "': "
       << sqlite3_errmsg(conn.connection());
    throw db::Exception(os.str());
  }

  // Reset position for next binds
  pos = 0;
  return *this;
}