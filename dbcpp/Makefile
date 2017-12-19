SRC = $(wildcard *.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))
LIB = libdbcpp.a

$(LIB): $(OBJ)
	ar rvs $(LIB) $(OBJ)

clean:
	rm -f lib*a *.o

%.o: %.cc
	$(CXX) -std=c++11 -c -I. -Wall $<
