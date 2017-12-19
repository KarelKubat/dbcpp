#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::disconnect() {
  if (db && usage_map.count(db) > 0) {
    --usage_map[db];
    if (usage_map[db] == 0)
      sqlite3_close(db);
    db = 0;
  }
}
