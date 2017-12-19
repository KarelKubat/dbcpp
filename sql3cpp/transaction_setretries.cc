#include <sql3cpp.h>

using namespace sql3;

void Transaction::setretries(int retr) {
  busy_retries = retr;
}
