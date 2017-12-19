#include <sql3cpp.h>

using namespace sql3;

void Transaction::setretrywait(int waitms) {
  busy_wait_ms = waitms;
}
