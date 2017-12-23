#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

int Transaction::affectedrows() {
  check_error();
  
  return affected_rows;
} 
