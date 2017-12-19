#include <sql3cpp.h>

using namespace sql3;

void Connection::check_connected() const {
  if (!connected())
    throw db::Exception("connection is not connected");
}  
