#include <cassert>
#include <sql3cpp.h>

int main() {
  sql3::Connection c;
  c.connect(":memory:");

  db::Transaction *t = new sql3::Transaction(c);
  // Create a table
  t->prepare("CREATE TABLE test (a INTEGER)").execute();

  // Table doesn't return anything
  t->prepare("SELECT * FROM test");
  assert(t->execute() == false);

  // SELECT COUNT
  t->prepare("SELECT COUNT(*) FROM test");
  assert(t->execute() == true);
  assert(t->colint(0) == 0);
  assert(t->execute() == false);

  delete t;

  return 0;
}
    
