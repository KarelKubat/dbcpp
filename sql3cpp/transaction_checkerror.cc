#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Transaction::check_error() {
  if (!conn.connected())
    throw db::Exception("transaction is not connected");
  if (error_seen)
    throw db::Exception("transaction is in error state, cannot be reused");
}
