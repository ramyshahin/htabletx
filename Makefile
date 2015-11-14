COMPOPT = -std=c++11 -mrtm -g
CC = g++

test0: tests/test0.cpp hashtable.h
	$(CC) $(COMPOPT) tests/test0.cpp -o test0

clean: 
	rm *.o test0
 
