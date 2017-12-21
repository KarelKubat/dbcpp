# dbcpp

dbcpp is my project to implement a consistent, transaction-aware API for
relational databases.

This project contains:

*  [dbcpp/](dbcpp/): The interface definition, which defines the classes
   `db::Transaction` and `db::Exception`

*  [sql3cpp/](sql3cpp/): Implementation of the interface for Sqlite3 in
   a class `sql3::Transaction` (and an auxiliary class `sql3::Connection`)

*  [examples/](examples/) which shows how to use this.

## Testing and Installation

If you want to try out this package, proceed as follows:

*  In this directory, type `make`. That compiles all sources and produces the
   libraries, but installs nothing.

*  Go to the [examples/](examples/) and run them. They can be compiled and
   executed without having to install anything.

For installation, type `make install`. This assumes the following destinations:

*  `/usr/local/lib` for built libraries,
*  `/usr/local/bin` for header files.

If you wish to change these paths during installation, then you may invoke
`make` as follows:

```shell
BIDIR=/where/ever/lib INCDIR=/where/ever/inc make install
```

## Structure of Your Own Programs

There are two approaches on structuring your own programs:

1. You may want to use a particular database backend, say package `sql3` for 
   Sqlite3,
   
1. You may want to stay backend-independent and decide at runtime with which
   database you want to connect.
   
### Preparing for Just One Backend

If your program only ever needs to access one backend, then you may use
simple objects. For example, in the case of Sqlite3:

```c++
#include <sql3cpp.h>

sql3::Connection conn("/where/ever/file.db");
sql3::Transaction trx(conn);

trx.prepare("INSERT INTO mytable (birtyear, name) VALUES (?, ?)")
   .bind(1412)
   .bind("Jeanne d'Arc")
   .execute();
```

### Preparing for Runtime Backend Choice

If your program would need to connect to any available backend, then you
may use the class `db:Transaction` to execute against any backend. That will
require access pointers:

```c++
#include <sql3cpp.h>
#include <pgcpp.h>                   // not yet implemented but to come

enum { USE_SQLITE3, USE_POSTGRES, ... } Backend;
Backend b;

...

db::Transaction *trx;
switch (b) {
  case USE_SQLITE3:
    trx = new sql3::Transaction(sql3::Connection("/where/ever/file.db");
    break;
  case USE_POSTGRES:
    trx = new pg::Transaction(...);  // not yet implemented but to come
    break;
}

trx->prepare("INSERT INTO mytable (birtyear, name) VALUES (?, ?)")
   ->bind(1412)
   ->bind("Jeanne d'Arc")
   ->execute();
   
...

delete trx;
```

### Linking Your Programs

Note that in the linkage stage of your programs you will need the appropriate
library for each back end that you are planning to use. For example, for Sqlite3
you would need the flags `-lsql3cpp -lsqlite3`. Here `-lsql3cpp` is the library
for this package, `-lsqlite3` is Sqlite3's own API.

## License

All this is distributed under a [GPLV3
license](https://www.gnu.org/licenses/gpl-3.0.en.html), which basically means
that you are free to do whatever you want with the code and that you are free
to modify and use the code as you see fit - as long as you make the changes
accessible to others under the same license. As a courtesy, please consider
informing me of any changes; I might incorporate them in this repository.

## Not Implemented Features

The database API knowingly only implements three data types for integer numbers
(represented in C++ as `int`), floating-point numbers (represented as `double`)
and text (represented as `std::string`). Other data types, such as boolean, date
or time, have not been implemented. The reason for this is that this package
limits itself to the smallest common denominator, which is (as far as I know)
Sqlite3.

The missing data types can however be easily emulated in the code; e.g., a date
can be represented as a string, a boolean as an integer.

## TODOs (Wishlist)

The roadmap for this project contains the following items. Feel free to chip
in if you are interested!

1.  Implement date and time in the C++ world and map to primitives in the
    database world.

2.  Implement backend for Postgresql. This can be (mostly) implemented just
    like `sql3::Transaction`.
