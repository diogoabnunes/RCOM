CC = gcc
CFLAGS = -Wall
DEPS = macros.h
OBJ = 
TARGETS = emissor recetor

all: emissor recetor

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

emissor: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ)

recetor: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ)

clean:
	@rm $(TARGETS)