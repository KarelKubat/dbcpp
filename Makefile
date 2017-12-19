DIRS = dbcpp sql3cpp sql3cpp/tests

all:
	@for d in $(DIRS); do \
		echo ; \
		echo "Making in subdir: $$d"; \
		make -C $$d || exit 1; \
	 done

clean:
	@for d in $(DIRS); do \
		echo ; \
		echo "Cleaning in subdir: $$d"; \
		make -C $$d clean || exit 1; \
	 done
