#include <chrono>
#include <thread>
#include <sstream>

#include <sql3cpp.h>

using namespace sql3;

Transaction &Transaction::prepare(std::string const &sql) {
  int ret;
    
  // Make sure that we are not already in an error state.
  check_error();

  // Start transaction if not done yet.
  conn.begin_transaction();

  for (int i = 0; i < busy_retries; ++i) {
      ret = sqlite3_prepare_v2(conn.connection(), sql.c_str(), sql.length(),
			       &stmt, 0);
      if (ret != SQLITE_BUSY)
	  break;
      std::this_thread::sleep_for(std::chrono::milliseconds(busy_wait_ms));
  }
  if (ret) {
      conn.rollback_transaction();

      std::ostringstream os;
      os << "failed to prepare '" << sql << "': "
	 << sqlite3_errstr(ret) << ", code " << ret;
      throw db::Exception(os.str());
  }

  // Reset position for next binds and remember the SQL statement
  pos = 0;
  last_sql = sql;
  return *this;
}
