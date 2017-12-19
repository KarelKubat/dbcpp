SRC = $(wildcard *.cc)
TST = $(patsubst %.cc, %.t, $(SRC))

all: $(TST)
	@echo "Running all tests!"
	@for t in $(TST) ; do  \
		echo "    ... $$t"; \
		$$t || exit 1; \
	 done

%.t: %.cc
	$(CXX) -I.. -I../../dbcpp -g -Wall -o $@ $< \
	  -L../../dbcpp -ldbcpp -L.. -lsql3cpp -lsqlite3

clean:
	rm -rf *.t *.dSYM/

# extra deps
connection.t: connection.cc ../libsql3cpp.a ../sql3cpp.h
emptytable.t: emptytable.cc ../libsql3cpp.a ../sql3cpp.h
unitofwork.t: unitofwork.cc ../libsql3cpp.a ../sql3cpp.h
lastid.t:     lastid.cc ../libsql3cpp.a ../sql3cpp.h