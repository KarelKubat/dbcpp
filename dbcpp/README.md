# db: General backend independent functions

Package `db` implements two things:

*  Exceptions that all backend-dependent implementations throw
*  Class `db::Transaction` that defines how all backend implementations run
   queries and how commit control is organized.

In order to compile this, you can type `make`. Invoking `make clean` removes
build artifacts.

## db::Exception

`db::Exception` is the overall exception type. It is used to signal errors
in all packages and is derived from `std::exception`, so that method
`what()` returns a printable representation. Example:

```c++
try {
  mytransaction->prepare(...)->execute();
} catch (std::exception const &e) {
  std::cerr << "Oops: " << e.what() << '\n';
  exit(1);
}
```

## db::Transaction

`db::Transaction` is a pure virtual class (interface) that prescribes how
transactions are implemented, irrespective of the database backend. It offers:

*  Preparing statements, which also starts a unit of work (transaction)
*  Binding variables
*  Execution
*  Retrieving columns after `select` statements
*  Retrieving the number of affected rows after `insert` or `update`
   statements
*  Retrieving the last autoincrement ID after `insert` statements on
   a table having an auto-incremented primary key
*  Finishing the unit of work, which is also done automatically when the
   destructor fires.

Commit control over transactions is enforced in the following way:

*  When method `finish()` is called, or when a `db::Transaction` object
   ceases to exist, then the current transaction is either rolled back or it
   is committed.
*  Whether to rollback or to commit depends on errors during execution.

Class `db::Transaction` offers no constructors. These are tightly bound to the
type of database backend. This class only prescribes how databases may be
accessed and how commit control is enforced.

### Typical SELECT example

The following is a typical example of preparing a `SELECT` statement and
executing it as long as rows are returned.

```c++
// Transaction handler
db::Transaction *trx;

// Construction depends on the used back end. For sqlite3 this might be:
sql3::Connection conn("/where/ever.db");
trx = new sql3::Transaction(conn);

// Preparing a statement, with variable binding:
trx->prepare("SELECT a, b, c FROM TABLE WHERE d=? AND e=? AND f=?")
   ->bind(12)
   ->bind("hello")
   ->bind(3.14);
while (trx->execute())
  std::cout << "a = " << trx->colint(0) << '\n'
	        << "b = " << trx->colstr(1) << '\n'
	        << "c = " << trx->coldouble(2) << '\n';
```

Note how `bind()` and be chained to `prepare()` and to other `bind()`s. In
fact it can also be chained to `execute()`, but this makes no sense in this
case as `execute()` is run in a loop.

The first binding binds a value to the first `?` in the SQL statement, the
second binds a value to the second, etc.. Binding is implemented for the types
`int`, `double` and for `std:string`. Other types may follow.

Method `execute()` will return `true` as long as there are rows to fetch. To
extract values from fetched rows, methods `colint()`, `colstr()` and
`coldouble()` are used. The parameter to these methods is the column number.

The column number has a default value of 0 which is convenient for `select`
statements that return just one value, as in the following example which also
shows how `execute()` can be chained.

```
trx->prepare("SELECT COUNT(*) FROM users WHERE name=?")
   ->bind("Jeanne d'Oh")
   ->execute();
std::cout << "Found " << trx->colint() << " times\n";
```

### Typical INSERT example: last auto-increment ID

Auto-increment primary keys are often used in tables, so that they may be
reused in other tables as foreign keys. This is returned by the method
`lastid()`. Consider the following schema where the primary key of a table
`employee` is used as a foreign key in `address`:

```sql
create table employee (
  employee_id primary key autoincrement,
  name text
);

create table address (
  street text,
  city text,
  employee_id integer references employee(employee_id)
);
```

In order to insert a name and address, one would need the next sequence number
of the primary key of `employee`, in order to use it as foreign key in
`address`:

```c++
db::Transaction *trx;

...

trx->prepare("insert into employee (name) values (?)"
   ->bind("Jeanne d'Oh")
   ->execute();
int id = trx->lastid();
trx->prepare("insert into address (street, city, employee_id) "
             "values (?, ?, ?)")
   ->bind("Rue d'Eglise")->bind("Orleans")->bind(id)
   ->execute();
```

### Typical UPDATE example: number of affected rows

Verifying that an update operation affects one or more rows can be important
in e.g. systems where one process adds work items into a queue and where
several workers get items from the queue and where it is necessary to ensure
that only one worker picks up an item (and not two or more).

This example assumes that there is a table `queue` with the following
layout:

```sql
create table queue (
  queue_id integer primary key autoincrement,   -- ID of this entry
  waiting  integer default 1,                   -- 1 if waiting for pickup
  action   text                                 -- task to run
);
```

Column `waiting` is used as sentinel; `action` is some arbitrary task
that a worker would need to perform and is not further described here.

In order to add items to the queue, the code would be:

```c++
db:Transaction *trx;
std::string mytask;
...
trx->prepare("insert into queue (action) values (?)
   ->bind(mytask)
   ->execute();
std::cout << "Added task " << mytask << " to the queue as id "
          << trx->lastid() << '\n';
```

In order to fetch a task from the queue, a consumer would get an ID and
action description and would set the `waiting` flag to zero -- but
only if `waiting` was 1 in the first place. (The actual processing of a
task is shown here as a function `process(int, std::string)` which is not
further detailed.)

```c++
db::Transaction *select_trx;
db::Transaction *update_trx;
std::string newtask;
int task_id;
...
// Fetch candidate tasks
select_trx->prepare("select queue_id, action from queue "
                    "where waiting=1 "
                    "order by queue_id");
while (trx->execute()) {
  task_id = select_trx->colint(0);
  newtask = select_trx->colstr();

  // Verify that it hasn't been locked by a parallel process, if not,
  // lock it and it's ours.
  // The addition "where waiting=1" makes this update atomic.
  update_trx->prepare("update queue "
                      "set waiting=0 "
                      "where queue_id=? and waiting=1")
     ->bind(task_id)
     ->execute();
  if (update_trx->affectedrows() > 0) {
    std::cout << "Got a lock on task ID " << task_id << ", processing\n");
    process(task_id, newtask);
  } else
    std::cout << "Candidate " << task_id << " was already taken\n";
}
```

A full working example is provided in the distribution as
[producer-consumer-queue](../examples/producer-consumer-queue/README.md).

### Controlling the Units of Work

Commit control is most 'natural' on a per-instance level of `db::Transaction`,
which means that database changes are either committed or rolled back
when a `db::Transaction` object is destroyed:

```c++
db::Transaction *trx;
...

try {
  trx->prepare("insert into mytable (a, b, c) values (?, ?, ?)")
     ->bind(1)->bind(2)->bind(3)
     ->execute();
  // Transaction has succeeded but is not committed yet
} catch (db::Exception const &e) {
  // Transaction failed but is not rolled back yet
  std::cout << e.what() << '\n';
}

// Transaction is now committed or rolled back, depending on the
// success during execution
delete trx;
```

A transaction object can be reused for subsequent `prepare()` / `execute()`
cycles while it is not in an error state. Once an execution throws an error,
the object is marked for rollback and next reuses are not permitted:

```c++
// This is correct. The transaction object is reused and all statements
// will either succeed or fail.

db::Transaction *trx;

...

try {
  trx->prepare("insert into mytable (a, b, c) values (?, ?, ?)")
     ->bind(1)->bind(2)->bind(3)
     ->execute();
  trx->prepare("insert into mytable (a, b, c) values (?, ?, ?)")
     ->bind(4)->bind(5)->bind(6)
     ->execute();
  trx->prepare("insert into mytable (a, b, c) values (?, ?, ?)")
     ->bind(7)->bind(8)->bind(9)
     ->execute();
} catch (db::Exception const &e) {
  // An execution above has failed. All will be rolled back.
  std::cout << e.what() << '\n';
}

...

delete trx;
```

The following however is incorrect. Inside the `catch` block, the transaction
object is in an error state and may not be reused:

```c++
// This is incorrect. The transaction object cannot be reused in the catch
// block since it is in an error state.

db::Transaction *trx;
...

try {
  trx->prepare("insert into mytable (a, b, c) values (?, ?, ?)")
     ->bind(1)->bind(2)->bind(3)
     ->execute();
  trx->prepare("insert into mytable (a, b, c) values (?, ?, ?)")
     ->bind(4)->bind(5)->bind(6)
     ->execute();
  trx->prepare("insert into mytable (a, b, c) values (?, ?, ?)")
     ->bind(7)->bind(8)->bind(9)
     ->execute();
} catch (db::Exception const &e) {
  std::cout << "There are now "
            << trx->prepare("select count(*) from mytable")
                  ->execute()->colint()
            << " rows in mytable\n";
}
...
delete trx;
```

Destroying transaction objects in order to force commit control may not always
be the most elegant approach. For this method `finish()` exists which:

* Either commits or rolls back the transaction
* Resets any error state.

For example:

```c++
// Insert some values
trx->prepare("insert into mytable (a, b, c) values (?, ?, ?)")
   ->bind(1)->bind(2)->bind(3)
   ->execute();

// Make sure the above is committed even when the below would fail
trx->finish();

// Insert some more
trx->prepare("insert into mytable (a, b, c) values (?, ?, ?)")
   ->bind(4)->bind(5)->bind(6)
   ->execute();
```
