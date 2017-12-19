#include <chrono>
#include <thread>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

bool Transaction::execute() {
  // Make sure that we are not already in an error state.
  check_error();

  // Incase of database busy, retry up to configured nr of times.
  int ret;
  for (int i = 0; i < busy_retries; ++i) {
    if ( (ret = sqlite3_step(stmt)) != SQLITE_BUSY )
      break;
    std::this_thread::sleep_for
      (std::chrono::milliseconds(busy_wait_ms * 1000));
  }

  // Check for errors
  if (ret != SQLITE_ROW && ret != SQLITE_DONE) {
    conn.rollback_transaction();
      
    std::ostringstream os;
    os << "execution failure: " << sqlite3_errmsg(conn.connection());
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
