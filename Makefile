CXX = gcc
CXXFLAGS = -Wall

all: all_tests

all_tests: all_tests.o
	$(CXX) $(CXXFLAGS) all_tests.o -o all_tests -lpthread

clean:
	rm -rf *.o all_tests

