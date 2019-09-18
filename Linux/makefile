
THING = g++ -fopenmp -I../src/ -D_DEBUG -c

build:
	$(THING) ../src/main.cpp
	$(THING) ../src/parsing/parsing.cpp
	$(THING) ../src/util/util.cpp
	$(THING) ../src/util/string.cpp
	g++ -fopenmp -o FuckMake main.o parsing.o util.o string.o
	rm main.o
	rm parsing.o
	rm util.o
	rm string.o