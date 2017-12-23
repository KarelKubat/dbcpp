#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::connect(std::string const &fname) {
  int ret;

  // Disconnect current connection, if any.
  disconnect();

  // Connect to the new source.
  if ( (ret = sqlite3_open(fname.c_str(), &db)) ) {
    std::ostringstream os;
    os << "cannot connect to '" << fname << "': " << sqlite3_errmsg(db)
       << ", code " << ret;
    throw db::Exception(os.str());
  }
}
