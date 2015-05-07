all: crack

crack: crack.o
	g++ -std=c++0x crack.o -o crack

crack.o: crack.cpp
	g++ -std=c++0x -c crack.cpp

clean:
	rm *o crack