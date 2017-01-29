CC=g++
CFLAGS=-c
LD=g++
LFLAGS=--std=c++14
OBJECTS=ip_utility.o hsocket.o

tests: test.bin
	@( ./test.bin && \
	echo "All tests passed" )&\
	echo -n "UDP socket test ok" | nc 127.0.0.1 1337 -u

test.bin: ${OBJECTS} test.o
	${LD} ${OBJECTS} test.o -o test.bin

test.o: tests/main.cpp
	${CC} ${CFLAGS} tests/main.cpp -o test.o

%.o : %.cpp %.h
	${CC} ${CFLAGS} $< -o $@
