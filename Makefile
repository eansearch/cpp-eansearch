all: example

example.o: example.cpp eansearch.hpp
	$(CXX) -c example.cpp

example: example.o
	$(CXX) example.o -o $@ -lssl -lcrypto

clean:
	rm example *.o

