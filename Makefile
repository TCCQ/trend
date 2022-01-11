CD:=-g 
LD:=-g 
W:=-Wall -Wextra 
FST:=-O3
LTNG:=-Ofast

CARG:=-std=gnu++20 -I/usr/include/libdrm/ -finline-functions -pthread
LARG:=-std=gnu++20 -ldrm -pthread -lpthread

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

all:main.o projection.o util.o fb.o timage.o matrix.o tpool.o
	g++ ${LARG} $^ -o $@

main.o:main.cc util.hh projection.hh fb.o
	g++ -c ${CARG} $< -o $@

projection.o:projection.cc projection.hh util.hh fb.o
	g++ -c ${CARG} $< -o $@

sdlfb.o:sdlfb.cc sdlfb.hh
	g++ -c ${CARG} $< -o $@

util.o:util.cc util.hh
	g++ -c ${CARG} $< -o $@

timage.o:timage.cc timage.hh
	g++ -c ${CARG} $< -o $@

matrix.o:matrix.cc matrix.hh util.hh
	g++ -c ${CARG} $< -o $@

fb.o:fb.cc fb.hh
	g++ -c ${CARG} $< -o $@

tpool.o:tpool.cc tpool.hh
	g++ -c ${CARG} $< -o $@

clean:
	rm -f all *.o gmon.out *.gcov *.gcno *.gcda
