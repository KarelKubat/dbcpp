all: top2000 top2000.db
	./top2000 analyze

top2000: top2000.cc
	$(CXX) -std=c++11 -I../../dbcpp -I../../sql3cpp -o top2000 top2000.cc \
	       -L../../sql3cpp -lsql3cpp -lsqlite3

top2000.db: top2000.sql
	rm -f top2000.db
	sqlite3 top2000.db < top2000.sql
	perl loadlist.pl top2000list.txt

clean:
	rm -rf top2000.db top2000 *.dSYM
