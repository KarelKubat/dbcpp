#include <sql3cpp.h>

using namespace sql3;

Transaction::~Transaction() {
  finish();
}
