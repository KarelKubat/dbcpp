SRC = $(wildcard *.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))
LIB = libdbcpp.a

$(LIB): $(OBJ)
	ar rvs $(LIB) $(OBJ)

clean:
	rm -f lib*a *.o

%.o: %.cc
	$(CXX) -c -I. -Wall $<
