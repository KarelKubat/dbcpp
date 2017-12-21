# dbcpp

dbcpp is my project to implement a consistent, transaction-aware API for
relational databases. This project contains:

*  [dbcpp/](dbcpp/): The interface definition, which defines the classes
   `db::Transaction` and `db::Exception`

*  [sql3cpp/](sql3cpp/): Implementation of the interface for Sqlite3 in
   a class `sql3::Transaction` (and an auxiliary class `sql3::Connection`)

*  [examples/](examples/) which shows how to use this.

So far the only concrete implementation of `db::Transaction` is for Sqlite3.  It
is until now the only usage that I needed. I plan to add a Postgresql
implementation soon(ish).

