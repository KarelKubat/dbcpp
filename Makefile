# Where to install the libs and headers.
LIBDIR ?= /usr/local/lib
INCDIR ?= /usr/local/include

# The rest should need no tweaking.
DIRS     = dbcpp sql3cpp sql3cpp/tests
EXAMPLES = examples/producer-consumer-queue examples/top-2000-songs-2016-in-NL

all:
	@for d in $(DIRS); do \
		echo ; \
		echo "Making in subdir: $$d"; \
		make -C $$d || exit 1; \
	 done

install:
	@for d in $(DIRS); do \
		echo ; \
		echo "Installing from subdir: $$d"; \
		LIBDIR=$(LIBDIR) INCDIR=$(INCDIR) \
		  make -C $$d install || exit 1; \
	 done

clean:
	@for d in $(DIRS) $(EXAMPLES); do \
		echo ; \
		echo "Cleaning in subdir: $$d"; \
		make -C $$d clean || exit 1; \
	 done
