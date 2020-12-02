CC = gcc
CFLAGS = -Wall
DEPS = utils.h
OBJ = app.c utils.c

all: download

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

download: $(OBJ)
	$(CC) $(OBJ) -o $@

clean:
	-rm -f download