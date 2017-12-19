#include <sql3cpp.h>

using namespace sql3;

int Transaction::getretries() const {
  return busy_retries;
}
