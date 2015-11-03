all: app
app: \
	src/app.o \
	src/main.o

.PHONY: clean
clean:
	rm -f *.o app

CFLAGS=-std=c99 -lwiringPi -Wall -Wextra -pedantic
