#include <dbcpp.h>

using namespace db;

Exception::Exception(std::string const &d)
  : desc(d) {
}
