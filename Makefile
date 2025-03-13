CC = gcc
CFLAGS = -Wall -Wextra -g

CFILES = panopticon.c themeSwitcher.c cJSON.c logger.c
OBJECTS = panopticon.o themeSwitcher.o cJSON.o logger.o

BINARY = panopticon

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
