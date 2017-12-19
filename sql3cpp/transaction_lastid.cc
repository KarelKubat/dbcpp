#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

/*
 * This is an ugly implementation as it is inherently not thread-safe.
 * However, we can't reuse the standard stmt data member as it may be
 * still in use and we can't create a new one on the same connection. And
 * we can't start another connection as that would break commit control.
 *
 * According to the sqlite3 docs, fetching the last ID isn't threadsafe
 * anyway (an INSERT in a parallel thread will overwrite our last ID anyway)
 * so not being thread-safe here is probably ok-ish.
 *
 * It's just a limitation of sqlite3.
 */

#define SQL "SELECT last_insert_rowid()"

static int id;
static int callback(void *unused, int ac, char **av, char **cols) {
  if (ac < 1)
    id = 0;
  else
    id = atoi(av[0]);
  return 0;
}

int Transaction::lastid() {
  std::ostringstream os;
  char *errmsg;
  
  check_error();
  id = 0;
  if (sqlite3_exec(conn.connection(), SQL, callback, 0, &errmsg)) {
    os << "failed to exec statement to fetch lastid: " << errmsg;
    throw db::Exception(os.str());
  }
  return id;
}
