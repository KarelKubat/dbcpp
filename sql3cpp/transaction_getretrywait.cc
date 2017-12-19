#include <sql3cpp.h>

using namespace sql3;

int Transaction::getretrywait() const {
  return busy_wait_ms;
}
