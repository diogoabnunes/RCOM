CC = gcc
CFLAGS = -Wall
DEPS = macros.h
OBJ = 
TARGETS = emissor recetor

all: emissor recetor

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

emissor: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm

recetor: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm

clean:
	@rm $(TARGETS)