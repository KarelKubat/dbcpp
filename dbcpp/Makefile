SRC = $(wildcard *.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))
HDR = dbcpp.h

all: $(OBJ)

# Installation only installs the header. The objects are packaged-in with
# each concrete database backend.
install: $(INCDIR)/$(HDR)

$(INCDIR)/$(HDR): $(HDR)
	cp $< $@

clean:
	rm -f *.o

%.o: %.cc
	$(CXX) -std=c++11 -c -I. -Wall $<
