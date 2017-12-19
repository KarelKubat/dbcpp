/*
 * dbcpp.h: Interface for DB handling.
 * Also defines a db-related exception class.
 */

#ifndef _DBCPP_H_
#define _DBCPP_H_

#include <exception>
#include <string>

namespace db {

  /*
   * Custom exceptions
   */
  class Exception: public std::exception {

  public:
    Exception(std::string const &desc);
    ~Exception() _NOEXCEPT;
    char const *what() const _NOEXCEPT;

  private:
    std::string desc;
  };

  /*
   * Transactions: units of work
   */
  class Transaction {
  public:
    virtual ~Transaction();

    virtual Transaction &prepare(std::string const &sql) = 0;

    virtual Transaction &bind(int val) = 0;
    virtual Transaction &bind(double val) = 0;
    virtual Transaction &bind(std::string const &str) = 0;

    virtual bool execute() = 0;

    virtual int colint(int pos = 0) = 0;
    virtual double coldouble(int pos = 0) = 0;
    virtual std::string colstr(int pos = 0) = 0;

    virtual int affectedrows() = 0;
    virtual int lastid() = 0;

    virtual void finish() = 0;
  };

} // namespace db


#endif // _DBCPP_H_
