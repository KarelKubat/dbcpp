#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

std::string Transaction::colstr(int col) {
  return (char*)sqlite3_column_text(stmt, col);
}
