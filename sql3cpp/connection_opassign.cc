#include <sql3cpp.h>

using namespace sql3;

Connection const &Connection::operator=(Connection const &other) {
    if (this != &other) {
	disconnect();
	db = other.db;
	increfcount();
    }
    return *this;
}
