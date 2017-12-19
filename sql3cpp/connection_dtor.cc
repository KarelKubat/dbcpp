#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

Connection::~Connection() {
  disconnect();
}
