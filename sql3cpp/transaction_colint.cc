#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

int Transaction::colint(int col) {
  return sqlite3_column_int(stmt, col);
}
