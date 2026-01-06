all: example

eansearch.o: eansearch.cpp eansearch.hpp
	$(CXX) -c eansearch.cpp

example.o: example.cpp eansearch.hpp
	$(CXX) -c example.cpp

example: example.o eansearch.o
	$(CXX) example.o eansearch.o -o $@ -lssl -lcrypto

clean:
	rm -rf example *.o cov-int*

