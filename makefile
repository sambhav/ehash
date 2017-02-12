ehash: main.o ext_hash.o
main.o: 
	g++ -c main.cpp ehash.h
ext_hash.o: 
	g++ -c ext_hash.cpp ehash.h 