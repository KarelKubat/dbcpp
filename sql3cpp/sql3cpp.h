/*
 * sql3cpp: Wrapping of sqlite3 for C++
 */

#ifndef _SQL3CPP_H_
#define _SQL3CPP_H_

#include <exception>
#include <map>
#include <string>

#include <sqlite3.h>

#include <dbcpp.h>

namespace sql3 {

  /*
   * Database connection
   */
  class Connection {
  public:
    Connection();
    Connection(std::string const &fname);
    ~Connection();

    void connect(std::string const &fname);
    void disconnect();
    bool connected() const;

    void begin_transaction();
    void end_transaction();
    void rollback_transaction();

    sqlite3 *connection();

  private:
    void exec_sql(std::string const &sql);
    void check_connected() const;
    
    sqlite3 *db;
    static std::map<sqlite3*, int> usage_map;
    static std::map<sqlite3*, bool> in_transaction_map;
  };

  /*
   * Transaction blocks
   */
  class Transaction: public db::Transaction {
  public:
    Transaction(sql3::Connection const &conn,
		int busy_retries=5, int busy_wait_ms=100);
    Transaction(sql3::Connection *conn,
		int busy_retries=5, int busy_wait_ms=100);
    ~Transaction();

    void setretries(int retr);
    int getretries() const;
    void setretrywait(int waitms);
    int getretrywait() const;

    Transaction &prepare(std::string const &sql);

    Transaction &bind(int val);
    Transaction &bind(double val);
    Transaction &bind(std::string const &str);

    bool execute();

    int colint(int pos = 0);
    double coldouble(int pos = 0);
    std::string colstr(int pos = 0);

    int affectedrows();
    int lastid();

    void finish();

  private:
    sql3::Connection conn;
    sqlite3_stmt *stmt;
    int pos;
    int busy_retries, busy_wait_ms;
    bool error_seen;

    void check_error();
  };

} // namespace sql3


#endif // _SQL3CPP_H_