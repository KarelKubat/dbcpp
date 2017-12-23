#include <sql3cpp.h>

using namespace sql3;

Transaction::Transaction(sql3::Connection const &c, int retries, int wait): 
  conn(c), stmt(0), pos(0),
  busy_retries(retries), busy_wait_ms(wait),
  error_seen(false), affected_rows(0), last_sql("") {
}
