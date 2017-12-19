#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Transaction::finish() {
  if (error_seen)
    conn.rollback_transaction();
  else
    conn.end_transaction();
  error_seen = false;
}
