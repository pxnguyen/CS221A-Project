CXX = gcc
CXXFLAGS = -Wall

all: syscall_time create_time test

syscall_time: syscall_time.o conv.o
	$(CXX) $(CXXFLAGS) syscall_time.o conv.o -o syscall_time

create_time: create_time.o conv.o
	$(CXX) $(CXXFLAGS) create_time.o conv.o -o create_time
clean:
	rm -rf *.o syscall_time create_time

