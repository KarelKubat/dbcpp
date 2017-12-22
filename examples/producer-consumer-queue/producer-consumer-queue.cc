#include <chrono>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <thread>

#include <sql3cpp.h>

sql3::Connection *conn;

void setup() {
  sql3::Transaction t(conn);

  t.prepare("create table queue ("
            "  queue_id integer primary key autoincrement, "
            "  waiting  integer default 1, "
            "  task     text "
            ")").execute();
  std::cout << "Setup: queue created\n";
}

void producer() {
  sql3::Transaction t(conn);

  for (int i = 0; i < 100; ++i) {
    std::ostringstream os;
    os << "Task " << i;
    t.prepare("insert into queue (task) values (?)")
        .bind(os.str())
        .execute();
  }
  std::cout << "Producer: created 100 tasks\n";
}

void consumer() {
  try {
    sql3::Transaction sel(conn);

    sel.prepare("select   queue_id,task "
		"from     queue "
		"where    waiting = 1 "
		"order by queue_id");
    while (sel.execute()) {
      sql3::Transaction upd(conn);

      upd.prepare("update queue set waiting=0 where waiting=1 and queue_id=?")
        .bind(sel.colint(0))
        .execute();
      if (upd.affectedrows() > 0) {
	std::this_thread::sleep_for(
	    std::chrono::milliseconds(100 + rand() % 300));
	std::cout << "Thread " << std::this_thread::get_id()
		  << " has finished task " << sel.colstr(1) << '\n';
      }
    }
  } catch (std::exception const &e) {
    std::cerr << "Consumer exception: " << e.what() << '\n';
  }
}

int main() {
  try {
    conn = new sql3::Connection(":memory:");

    setup();
    producer();

    std::thread a(consumer);
    std::thread b(consumer);
    std::thread c(consumer);
  
    a.join();
    b.join();
    c.join();
  } catch (std::exception const &e) {
    std::cerr << "Main exception: " << e.what() << '\n';
    return 1;
  }
  
  return 0;
}
