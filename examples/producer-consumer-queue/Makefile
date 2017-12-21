all: producer-consumer-queue
	./producer-consumer-queue

producer-consumer-queue: producer-consumer-queue.cc
	$(CXX) -std=c++11 -I../../dbcpp -I../../sql3cpp \
	       -o producer-consumer-queue \
	       producer-consumer-queue.cc \
	       -L../../sql3cpp -lsql3cpp -lsqlite3

clean:
	rm -f producer-consumer-queue
