#include <cassert>
#include <iostream>

#include <sql3cpp.h>

sql3::Connection getconn() {
    static sql3::Connection conn("connpass.db");
    return conn;
}

void setup(sql3::Connection &conn) { 
    std::cout << "Setting up\n";
    
    sql3::Transaction trx(conn);
    trx.prepare("create table if not exists test (a integer)").execute();
    trx.prepare("delete from test").execute();
    trx.finish();
}

void addrows() {
    std::cout << "Adding 3 rows\n";
    
    sql3::Transaction trx(getconn());
    for (int i = 0; i < 3; ++i)
	trx.prepare("insert into test (a) values (?)")
	    .bind(i)
	    .execute();
    trx.finish();
}

int count() {
    std::cout << "Counting table rows\n";
    
    sql3::Transaction trx(getconn());
    trx.prepare("select count(*) from test").execute();
    return trx.colint();
}

int main() {
    sql3::Connection conn = getconn();
    setup(conn);

    addrows();
    assert(count() == 3);
    addrows();
    assert(count() == 6);

    return 0;
}
