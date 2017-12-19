#include <cassert>
#include <iostream>
#include <sql3cpp.h>

int main() {
  sql3::Connection c; 
  c.connect(":memory:");

  // Create a table without an autoincrement ID.
  sql3::Transaction t(c);
  t.prepare("CREATE TABLE testone (a TEXT)").execute();

  // Insert 3 rows
  t.prepare("INSERT INTO testone (a) VALUES('Hello')").execute();
  t.prepare("INSERT INTO testone (a) VALUES('World')").execute();
  assert(t.lastid() == 2);

  // Create a table with an autoincrement.
  t.prepare("CREATE TABLE testtwo ("
	    "  id integer primary key autoincrement, "
	    "  a  text "
	    ")").execute();
  for (int i = 1; i <= 10; ++i) {
    t.prepare("INSERT INTO testtwo (a) VALUES (?)")
      .bind("Hello World")
      .execute();
    assert(t.lastid() == i);
  }

  return 0;
}
    
