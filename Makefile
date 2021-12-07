CD:=-g 
LD:=-g 
W:=-Wall -Wextra 
FST:=-O3
LTNG:=-Ofast

CARG:=-std=gnu++17
LARG:=-std=gnu++17 -lSDL2

ifdef DEBUG 
	CARG:=${CARG} ${CD}
	LARG:=${LARG} ${LD}
endif

ifdef WARN 
	CARG:=${CARG} $W
endif

ifdef FAST
	CARG:=${CARG} ${FST}
	LARG:=${LARG} ${FST}
endif

ifdef LIGHTNING
	CARG:=${CARG} ${LTNG}
	LARG:=${LARG} ${LTNG}
endif

run:all
	./all

all:main.o projection.o util.o sdlfb.o timage.o matrix.o cubewrap.o
	g++ ${LARG} $^ -o $@

main.o:main.cc util.hh projection.hh sdlfb.hh cubewrap.hh
	g++ -c ${CARG} $< -o $@

projection.o:projection.cc projection.hh util.hh sdlfb.hh
	g++ -c ${CARG} $< -o $@

sdlfb.o:sdlfb.cc sdlfb.hh
	g++ -c ${CARG} $< -o $@

util.o:util.cc util.hh
	g++ -c ${CARG} $< -o $@

timage.o:timage.cc timage.hh
	g++ -c ${CARG} $< -o $@

matrix.o:matrix.cc matrix.hh util.hh
	g++ -c ${CARG} $< -o $@

cubewrap.o:cubewrap.cc cubewrap.hh util.hh 
	g++ -c ${CARG} $< -o $@

clean:
	rm -f all *.o gmon.out *.gcov *.gcno *.gcda
