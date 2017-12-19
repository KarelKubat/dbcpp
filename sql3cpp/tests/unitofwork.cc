#include <cassert>
#include <iostream>
#include <sql3cpp.h>

sql3::Connection c;

void setup() {
  sql3::Transaction t(c);
  t.prepare("CREATE TABLE test (a INTEGER)").execute();
}

void insert(int max) {
  sql3::Transaction t(c);

  t.prepare("INSERT INTO test (a) VALUES (?)");
  for (int i = 0; i < max; ++i) {
    t.bind(i);
    t.execute();
  }
}

void insert_and_fail(int max) {
  sql3::Transaction t(c);

  t.prepare("INSERT INTO test (a) VALUES (?)");
  for (int i = 0; i < max; ++i) {
    t.bind(i);
    t.execute();
  }
  try {
    t.prepare("INSERT INTO xyzzy (plugh) VALUES ('colossal', 'cave')");
    throw "Bad sql statement succeeded where it should have failed";
  } catch (db::Exception e) {
  }
}

void readback(int max) {
  sql3::Transaction t(c);

  t.prepare("SELECT a FROM test INC");
  int i = 0;
  while (t.execute())
    assert(t.colint(0) == i++);
  assert(i == max);
}

int main() {
  c.connect(":memory:");
  
  setup();
  insert(10);
  readback(10);
  insert_and_fail(5);
  readback(10);

  return 0;
}
