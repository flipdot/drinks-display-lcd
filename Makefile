all: app
app: app.c

.PHONY: clean
clean:
	-rm app

CFLAGS=-Wall -Wextra -pedantic
