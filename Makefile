CD:=-g 
LD:=-g 
W:=-Wall -Wextra 
FST:=-O3
LTNG:=-Ofast

CARG:=-std=gnu++20 -finline-functions -pthread
LARG:=-std=gnu++20 -pthread -lpthread

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

drm:main.o projection.o util.o drmfb.o timage.o matrix.o tpool.o grid.o
	g++ ${LARG} -ldrm $^ -o $@

sdl:main.o projection.o util.o sdlfb.o timage.o matrix.o tpool.o grid.o
	g++ ${LARG} -lSDL2 $^ -o $@

drmfb.o:drmfb.cc fb.hh
	g++ -c ${CARG} -I/usr/include/libdrm/ $< -o $@

sdlfb.o:sdlfb.cc fb.hh
	g++ -c ${CARG} $< -o $@

main.o:main.cc util.hh projection.hh fb.hh grid.hh
	g++ -c ${CARG} $< -o $@

projection.o:projection.cc projection.hh util.hh fb.hh
	g++ -c ${CARG} $< -o $@

util.o:util.cc util.hh
	g++ -c ${CARG} $< -o $@

timage.o:timage.cc timage.hh
	g++ -c ${CARG} $< -o $@

matrix.o:matrix.cc matrix.hh util.hh
	g++ -c ${CARG} $< -o $@

tpool.o:tpool.cc tpool.hh
	g++ -c ${CARG} $< -o $@

grid.o:grid.cc grid.hh
	g++ -c ${CARG} $< -o $@

clean:
	rm -f sdl drm *.o gmon.out *.gcov *.gcno *.gcda
