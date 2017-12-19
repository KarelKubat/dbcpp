all: top2000 top2000.db
	./top2000 analyze

top2000: top2000.cc
	$(CXX) -std=c++11 -I../dbcpp -I../sql3cpp -o top2000 top2000.cc \
	       -L../dbcpp -L../sql3cpp -lsql3cpp -ldbcpp -lsqlite3

top2000.db: top2000.sql
	rm -f top2000.db
	sqlite3 top2000.db < top2000.sql
	perl loadlist.pl top2000list.txt

clean:
	rm -f top2000.db top2000
