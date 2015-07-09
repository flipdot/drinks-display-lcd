all: app
app: app.c

.PHONY: clean
clean:
	-rm app

CFLAGS=-std=c99 -lwiringPi -Wall -Wextra -pedantic
