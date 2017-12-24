#include <sql3cpp.h>

using namespace sql3;

Connection::Connection(Connection const &other): db(other.db) {
    increfcount();
}
