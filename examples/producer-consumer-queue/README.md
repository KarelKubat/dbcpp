# A Producer-Consumer Queue

This example shows a simple producer/consumer queue approach, expressed
in Sqlite3. There are two main parts to this example:

*  One **producer** creates tasks and enters them into the queue.
*  Three **consumers** (threads) query the queue and pick up tasks to process.
   The example shows how the consumers compete for tasks, but never does more
   than one consumer pick up the same task.
   
Once a consumer picks up a task, it processes it, which is represented here
by waiting (sleeping) for 0.1 to 0.4 seconds and displaying on `std::cout` that
the task is done.

A table is used as the task queue with the following layout:

```sql
create table queue (
  queue_id integer primary key autoincrement,
  waiting  integer default 1,
  task     text
);
```

*  Column `queue_id` is just the primary key.
*  Column `waiting` indicates whether the task is waiting for pickup by a
   consumer.
*  Column `task` is just a descriptive text. In reality this might indicate
   what action to perform; e.g., which backup to make, which batch job to
   run, which URL to index, etc..
   
## main()

The `main()` function has the tasks to call `setup()` (which initializes the
database), to call `producer()` (which enters some tasks into the queue) and
to start three threads, each calling `consumer()`. Next, `main()` waits until
the three threads are finished, and exits. 

The database connection `conn` is a global variable in this example. In a real
situation it would probably be passed as a parameter, or each part (producer and
consumers) would create their own connection.

```c++
sql3::Connection *conn;

int main() {
  conn = new sql3::Connection(":memory:");

  setup();
  producer();

  std::thread a(consumer);
  std::thread b(consumer);
  std::thread c(consumer);

  a.join();
  b.join();
  c.join();

  return 0;
}
```

## setup() and producer()

Function `setup()` simply creates the queue table. Function `producer()` enters
tasks into the queue and leaves the column `waiting` to its default.

```c++
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
```

Note that the producer is ran only once at program start. A more real-life
example would need an endless loop in the producer that would enter tasks as
they are found, as opposed to creating 100 tasks and stopping.

## consumer()

Function `consumer()` is started as a thread, with concurrent consumer threads
running at the same time. Each consumer must try to pick up a task, but it
must make sure that it doesn't pick up a task that another consumer is also
working on. There is a race condition.

There is a number of possibilities to avoid racing:

*  The consumer function could use a mutex to obtain a lock, pick up a task,
   and then release the lock. A global lock variable would ensure that
   other threads wait while one thread is picking up a task. This is a valid
   approach for threads, but not for simultaneously running separate programs
   that cannot share a global lock variable.
*  In order to accomodate for concurrently running consumer programs, the lock
   could be in shared memory, or in a separate lock file. That would of course
   work.
*  But since we are talking about databases, let's avoid racing conditions
   using the database. 
   
Some databases offer possibilities to lock tables, but Sqlite3 doesn't. One
possible work-around, which is shown here, is the following.

1. Each consumer selects candidates for pickup using `select queue_id, task
   from queue where waiting = 1 order by queue_id`. The `where` clause 
   filters out already picked up tasks. The `order` clause ensures that the
   earliest tasks are picked up first (but this isn't required to make the
   consumer work correctly).
1. Each queue row in this list can be **potentially** picked up and processed,
   but only if its `waiting` column is still 1. Another consumer might have
   picked up this task already. To ensure that only one consumer takes the
   task, an atomic update is performed: `update queue set waiting=0 where 
   waiting=1 and queue_id=?`. If this update affects a row, then we can
   be sure that the waiting flag was still 1 at the time of the update, and
   hence we can be sure that no other consumer picked up the task as well.
   
The code that expresses this is the following:

```c++
void consumer() {
  sql3::Transaction sel(conn);

  sel.prepare("select queue_id,task "
              "from queue "
              "where waiting = 1 "
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
}
```

It should be noted that this consumer stops (returns) after one run. For this
example that's ok, because once the consumers start, no entries are added to the
queue. A real-life application would most likely

*  Run this forever (in a `while (true)` loop),
*  Pause (sleep) for some time between the loops.

## Trying It Out Yourself

The example is driven by [Makefile](Makefile) which creates the program
`producer-consumer-queue` from the source
[producer-consumer-queue.cc](producer-consumer-queue.cc). 

Invoking `make` builds the executable and runs it. Invoking `make clean` removes
artifacts.

Before trying this, make sure to invoke `make` from the top level directory to
make the necessary libraries. As prerequisite you must have the Sqlite3
develpment package installed, which is done on Debian using `apt-get install
libsqlite3-dev`).
