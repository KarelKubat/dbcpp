#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

bool Connection::connected() const {
  return db != 0;
}
