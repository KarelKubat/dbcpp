SRC = $(wildcard *.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))
LIB = libsql3cpp.a
HDR = sql3cpp.h

$(LIB): $(OBJ) dbcppobj
	ar rvs $(LIB) $(OBJ) ../dbcpp/*.o

dbcppobj:
	make -C ../dbcpp

install: $(LIBDIR)/$(LIB) $(INCDIR)/$(HDR)

$(LIBDIR)/$(LIB): $(LIB)
	cp $< $@
	chmod 644 $<

$(INCDIR)/$(HDR): $(HDR)
	cp $< $@
	chmod 644 $<

%.o: %.cc
	$(CXX) -std=c++11 -I. -I../dbcpp -c -Wall $<

tests: $(LIB) testrun

testrun:
	make -C tests all

clean:
	rm -f *.o $(LIB)
	make -C tests clean
