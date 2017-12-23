#include <chrono>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <thread>

#include <sql3cpp.h>

sql3::Connection *conn;

void setup() {
  sql3::Transaction t(conn);

  t.prepare("create table if not exists queue ("
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
  // We use a mutex in order to not have std::cout messages being sent
  // concurrently, which messes up output.
  static std::mutex cout_mutex;
  
  try {
    int tasks_handled = 0;
    sql3::Transaction sel(conn), upd(conn);

    sel.prepare("select count(*) from queue where waiting=1").execute();
    cout_mutex.lock();
    std::cout << "Thread " << std::this_thread::get_id() << ": there are "
	      << sel.colint() << " queue entries waiting for pickup\n";
    cout_mutex.unlock();

    sel.prepare("select   queue_id,task "
		"from     queue "
		"where    waiting = 1 "
		"order by queue_id");
    while (sel.execute()) {
      upd.prepare("update queue set waiting=0 where waiting=1 and queue_id=?")
        .bind(sel.colint(0))
        .execute();
      if (upd.affectedrows() > 0) {
	std::this_thread::sleep_for(
	    std::chrono::milliseconds(100 + rand() % 300));
	cout_mutex.lock();
	std::cout << "Thread " << std::this_thread::get_id()
		  << " has finished queue entry '" << sel.colstr(1) << "'\n";
	cout_mutex.unlock();
	tasks_handled++;
      }
    }
    
    cout_mutex.lock();
    std::cout << "Thread " << std::this_thread::get_id() << ": handled "
	      << tasks_handled << " queue entries\n";
    cout_mutex.unlock();

  } catch (std::exception const &e) {
    std::cerr << "Consumer exception: " << e.what() << '\n';
  }
}

void cleanup() {
  sql3::Transaction t(conn);
  t.prepare("delete from queue").execute();
}

int main() {
  try {
    conn = new sql3::Connection("queue.db");
    
    setup();
    producer();

    std::thread a(consumer);
    std::thread b(consumer);
    std::thread c(consumer);
  
    a.join();
    b.join();
    c.join();

    cleanup();
  } catch (std::exception const &e) {
    std::cerr << "Main exception: " << e.what() << '\n';
    return 1;
  }
  
  return 0;
}
