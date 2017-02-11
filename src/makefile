CC=g++
CFLAGS=-c
LD=g++
LFLAGS=--std=c++14
OBJECTS=ip_utility.o hsocket.o dns_packet.o message.o tunnel_dns.o convert_utility.o
EXT=app
WINE=


all: tests bin

win: CC=/usr/bin/x86_64-w64-mingw32-g++
win: CFLAGS=-c -DWIN32_LEAN_AND_MEAN
win: LD=/usr/bin/x86_64-w64-mingw32-g++
win: LDFLAGS=--std=c++14 -lws2_32 -lmsvcrt -lkernel32 -Lsubsystem,console -static-libstdc++ -static-libgcc -Wl,-Bstatic -lstdc++ -lpthread
win: EXT=exe
win: WINE=wine

win: all

winbin: CC=/usr/bin/x86_64-w64-mingw32-g++
winbin: CFLAGS=-c -DWIN32_LEAN_AND_MEAN
winbin: LD=/usr/bin/x86_64-w64-mingw32-g++
winbin: LDFLAGS=--std=c++14 -lws2_32 -lmsvcrt -lkernel32 -Lsubsystem,console -static-libstdc++ -static-libgcc -Wl,-Bstatic -lstdc++ -lpthread
winbin: EXT=exe
winbin: WINE=wine

winbin: bin
	

bin: server.${EXT} client.${EXT}

tests: test.${EXT}
	${WINE} ./test.${EXT}

test.${EXT}: ${OBJECTS} test.o
	${LD} ${OBJECTS} test.o -o test.${EXT} ${LDFLAGS}

client.${EXT}: ${OBJECTS} client.o
	${LD} ${OBJECTS} client.o -o client.${EXT} ${LDFLAGS}

server.${EXT}: ${OBJECTS} server.o
	${LD} ${OBJECTS} server.o -o server.${EXT} ${LDFLAGS}

test.o: tests/main.cpp
	${CC} ${CFLAGS} tests/main.cpp -o test.o

%.o : %.cpp %.h
	${CC} ${CFLAGS} $< -o $@

PHONY.: clean win winbin

clean:
	rm ${OBJECTS} test.o
