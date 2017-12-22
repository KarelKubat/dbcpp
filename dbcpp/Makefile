SRC = $(wildcard *.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))
HDR = dbcpp.h

all: $(OBJ)

# Installation only processes the header. The objects are packaged-in with
# each concrete database backend, see e.g. ../sql3cpp/Makefile.
install: $(INCDIR)/$(HDR)

$(INCDIR)/$(HDR): $(HDR)
	cp $< $@
	chmod 644 $<

clean:
	rm -f *.o

%.o: %.cc
	$(CXX) -std=c++11 -c -I. -Wall $<
