COOR_CC_SOURCES = $(wildcard coordinator.cc server.cc)
COOR_CC_HEADERS = $(wildcard coordinator.h server.h)

COOR_CC_OBJS = ${COOR_CC_SOURCES:.cc=.o}

CLI_CC_SOURCES = $(wildcard client.cc)
CLI_CC_HEADERS = $(wildcard client.h)

CLI_CC_OBJS = ${CLI_CC_SOURCES:.cc=.o}

CC = g++
CFLAGS = -std=c++14 -g -pthread

all: coordinator.out client.out

%.o: %.cc ${COOR_CC_HEADERS} ${CLI_CC_HEADERS}
	$(CC) -c -o $@ $< $(CFLAGS)

coordinator.out: ${COOR_CC_OBJS}
	$(CC) -o $@ $^ $(CFLAGS)
	rm coordinator.o server.o

client.out: ${CLI_CC_OBJS}
	$(CC) -o $@ $^ $(CFLAGS)
	rm client.o

server: coordinator.out
	./coordinator.out

client: client.out
	./client.out

clean:
	rm -rf *.o *.out
