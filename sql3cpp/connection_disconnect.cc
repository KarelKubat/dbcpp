#include <iostream>
#include <sstream>
#include <sql3cpp.h>

using namespace sql3;

void Connection::disconnect() {    
    if (decrefcount()) {
	
        #ifdef DEBUG
	std::cout << "connection " << db << " disconnected\n";
        #endif
	
	sqlite3_close(db);	
	db = 0;
    }
}
