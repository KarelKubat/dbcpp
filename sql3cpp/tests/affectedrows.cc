#include <cassert>
#include <sql3cpp.h>

int main() {
  sql3::Connection c; 
  c.connect(":memory:");
  
  db::Transaction *t = new sql3::Transaction(c);

  // Create a table
  t->prepare("CREATE TABLE test (a INTEGER)").execute();

  // Insert 3 rows
  for (int i = 1; i <= 3; i++) {
    t->prepare("INSERT INTO test (a) VALUES(1)").execute();
    assert(t->affectedrows() == 1);
  }

  // Update
  t->prepare("UPDATE test SET a = 2").execute();
  assert(t->affectedrows() == 3);

  delete t;

  return 0;
}
    
