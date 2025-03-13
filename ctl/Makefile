CC = gcc
CFLAGS = -Wall -Wextra -g

CFILES = optic-ctl.c
OBJECTS = optic-ctl.o

BINARY = optic-ctl

all: ${BINARY}

${BINARY}: ${OBJECTS}
	@${CC} -o $@ $^

%.o: %.c
	@${CC} ${CFLAGS} -c -o $@ $^

clean: ${BINARY}
	rm -rf *.o

install: ${BINARY}
	rm -rf *.o
	sudo mv ${BINARY} /usr/local/bin
