#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

double Transaction::coldouble(int col) {
  return sqlite3_column_double(stmt, col);
}

