#include <dbcpp.h>

using namespace db;

char const *Exception::what() const throw() {
  return desc.c_str();
}
