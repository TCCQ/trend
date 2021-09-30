CD:=-g -pg -coverage
LD:=-g -pg -coverage
W:=-Wall -Wextra 

CARG:=-std=gnu++17
LARG:=-std=gnu++17 -lSDL2

ifdef DEBUG 
	CARG:=${CARG} ${CD}
	LARG:=${LARG} ${LD}
endif

ifdef WARN 
	CARG:=${CARG} $W
endif


run:all
	./all

all:main.o projection.o util.o sdlfb.o 
	g++ ${LARG} $^ -o $@

main.o:main.cc util.hh projection.hh sdlfb.hh
	g++ -c ${CARG} $< -o $@

projection.o:projection.cc projection.hh util.hh sdlfb.hh
	g++ -c ${CARG} $< -o $@

sdlfb.o:sdlfb.cc sdlfb.hh
	g++ -c ${CARG} $< -o $@

util.o:util.cc util.hh
	g++ -c ${CARG} $< -o $@

clean:
	rm -f all *.o gmon.out *.gcov *.gcno *.gcda
