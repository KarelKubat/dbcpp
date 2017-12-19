# db::Exception

`db::Exception` is the overall exception type. It is used to signal errors
in all packages and is derived from `std::exception`, so that method
`what()` returns a printable representation. Example:

```c++
try {
  mytransaction.prepare(...)
} catch (std::exception const &e) {
  std::cerr << "Oops: " << e << '\n';
  exit(1);
}
```

# db::Transaction

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

Class `db::Transaction` offers no constructors. This is tightly bound to the
type of database backend. This class only prescribes how databases may be
accessed.

## Typical SELECT example

The following is a

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

Preparing and binding variables may be chained as in the example above. The
first binding binds the first variable, etc.. Binding is implemented for the
types `int`, `double` and for `std:string`. Other types may follow.

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

## Typical INSERT example: last auto-increment ID

## Typical UPDATE example: number of affected rows
