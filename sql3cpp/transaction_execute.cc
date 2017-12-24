#include <chrono>
#include <mutex>
#include <thread>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

bool Transaction::execute() {
  static std::mutex m;
  
  // Make sure that we are not already in an error state.
  check_error();

  // Incase of database busy, retry up to configured nr of times.
  int ret;
  for (int i = 0; i < busy_retries; ++i) {
    // The # of affected rows is on the connection level. We need to fetch
    // this as soon as the statement executes.
    m.lock();
    ret = sqlite3_step(stmt);
    affected_rows = sqlite3_changes(conn.connection());
    m.unlock();
    
    if (ret != SQLITE_BUSY)
      break;
    std::this_thread::sleep_for(std::chrono::milliseconds(busy_wait_ms));
  }

  // Check for errors
  if (ret != SQLITE_ROW && ret != SQLITE_DONE) {
    conn.rollback_transaction();

    std::ostringstream os;
    os << "execution failure: " << sqlite3_errstr(ret)
       << ", sql " << last_sql;
    throw db::Exception(os.str());
  }

  if (ret != SQLITE_ROW) {
    // No more rows. The prepared statement may be reused and binds will
    // again start at pos 0.
    pos = 0;
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    return false;
  }
  // More rows to come.
  return true;
}
