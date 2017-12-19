SRC = $(wildcard *.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))
LIB = libsql3cpp.a

$(LIB): $(OBJ)
	ar rvs $(LIB) $(OBJ)

%.o: %.cc
	$(CXX) -std=c++11 -I. -I../dbcpp -c -Wall $<

tests: $(LIB) testrun

testrun:
	make -C tests all

clean:
	rm -f *.o $(LIB)
	make -C tests clean
