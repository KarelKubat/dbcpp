#include <cassert>
#include <sql3cpp.h>

void func(sql3::Connection x) {
  sql3::Connection y;
  sql3::Connection z(x);

  y = x;

  assert(x.connection() == y.connection());
  assert(z.connection() == x.connection());
}

int main() {
  sql3::Connection c;

  c.connect(":memory:");
  assert(c.connected());

  sql3::Connection d(c);
  assert(d.connection() == c.connection());

  sql3::Connection e;
  e = d;
  assert(c.connection() == e.connection());
  assert(d.connection() == e.connection());

  func(c);

  assert(c.connected());
  assert(d.connected());
  assert(e.connected());

  return 0;
}
