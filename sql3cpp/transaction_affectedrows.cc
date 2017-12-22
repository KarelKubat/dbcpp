#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

int Transaction::affectedrows() {
  check_error();

  return sqlite3_changes(conn.connection());
}
