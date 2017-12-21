# sql3: db-compliant Implementation for Sqlite3

Package `sql3` implements the transactional database API for Sqlite3 as
specified by package `db`. Since Sqlite3 has commit-control on database
connection level, there is also a representation of such connections.

This description only shows the `sql3` specifics. For a more thorough
description of the API please refer to the package [dbcpp](../dbcpp/). There
are also [examples](../examples) that you can follow.

In order to compile this, you can type `make` in this directory, which creates
the library and runs tests. `make clean` removes artifacts. A prerequisite is
that the Sqlite3 development package is installed on your system. For
Debian-based systems this can be achieved using `apt-get install
libsqlite3-dev`. In order to install, use the top-level `Makefile` of this
project.

## sql3::Connection

In order to connect to a Sqlite3 database, the following options are
available:

```c++
// Default constructor and explicit call to connect()
sql3::Connection c;
c.connect("my.db");
```

```c++
// All in one
sql3::Connection("my.db");
```

The parameter `my.db` in the above examples is usually the diskfile where
Sqlite3 stores data. This string is passed literally to Sqlite3, and hence
supports a number of options:

Example                     | Means
----------------------------|-------------
`/path/my.db`               | File on disk
`:memory:`                  | In-memory db, separate for each `sql3::Connection`
`file:///path/my.db`        | Same as `/path/my.db`
`file:///path/my.db?mode=ro`| Read-only mode

Class `sql3::Connection` has a number of other public methods which are
used in `sql3::Transaction` (see below) but may be usable in another context.
If you use this class in connection with `sql3::Transaction`, which you 
most probably will, then using these methods is discouraged because this 
functionality is managed by the transaction interface.

*  `void disconnect()`: Commits or rolls back the transaction and disconnects
*  `bool connected() const`: Returns `true` when connected
*  `void begin_transaction(), end_transaction(), rollback_transaction()`:
   These may be used to forced commit control

## sql3::Transaction

### Constructors and Retries

The constructors take a reference or a pointer to a `sql3::Connection` so that
the database may be accessed and commit control may be enforced. Additionally,
there are two optional parameters:

*  `int busy_retries`: how many times an attempt should be taken to execute
   a statement when the database connection is busy (default: 5)
*  `int busy_wait_ms`: how many milliseconds wait time should be maintained
   between retries (default: 100).

Retrying and waiting between retries is a necessity for Sqlite3, which
implements only database-level locking (and not row-level). Therefore, multiple
programs accessing the same Sqlite3 database may have to wait for each others'
completion. These settings can also be manipulated with getters and setters. The
declaration of the constructors and of the retry-related methods is as follows:

```c++
    Transaction(sql3::Connection const &conn,
		int busy_retries=5, int busy_wait_ms=100);
    Transaction(sql3::Connection *conn,
		int busy_retries=5, int busy_wait_ms=100);
    ~Transaction();

    void setretries(int retr);
    int getretries() const;
    void setretrywait(int waitms);
    int getretrywait() const;
```

### Transaction-related methods

Besides the constructors shown above and retry-related methods, class
`sql3::Transaction` implements the API prescribed by `db::Transaction`.

Package `sql3` has the following transactional methods as required by its
base class `db::Transaction`:

*  `prepare(std::string)` to prepare a statement,
*  `bind(...)` to bind an integer, double or string to a placeholder in
   the statement,
*  `execute()` to run the statement,
*  `colint(int pos), coldouble(int pos), colstr(int pos)` to fetch an int,
   double or string from the result of a select statement at column `pos`,
*  `affectedrows()` that returns the count of affected rows in an insert
   or update statement,
*  `lastid()` that returns the value of the last autoincrement ID after an
   insert statement.
