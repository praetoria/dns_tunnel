CC=g++
CFLAGS=-c
LD=g++
LFLAGS=--std=c++14
OBJECTS=ip_utility.o hsocket.o dns_packet.o message.o tunnel_dns.o convert_utility.o
EXT=app
WINE=


all: tests

win: CC=/usr/bin/x86_64-w64-mingw32-g++
win: CFLAGS=-c -DWIN32_LEAN_AND_MEAN
win: LD=/usr/bin/x86_64-w64-mingw32-g++
win: LDFLAGS=--std=c++14 -lws2_32 -lmsvcrt -lkernel32 -Lsubsystem,console -static-libstdc++ -static-libgcc -Wl,-Bstatic -lstdc++ -lpthread
win: EXT=exe
win: WINE=wine

win: all
	

tests: test.${EXT}
	@${WINE} ./test.${EXT}

test.${EXT}: ${OBJECTS} test.o
	${LD} ${OBJECTS} test.o -o test.${EXT} ${LDFLAGS}

test.o: tests/main.cpp
	${CC} ${CFLAGS} tests/main.cpp -o test.o

%.o : %.cpp %.h
	${CC} ${CFLAGS} $< -o $@

PHONY.: clean win

clean:
	rm ${OBJECTS} test.o
