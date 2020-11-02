CC = gcc
CFLAGS = -Wall
DEPS = data_link.h state_machine.h defines.h utils.h
OBJ = data_link.c state_machine.c utils.c
TARGETS = app

all: app

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

app: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ)

clean:
	@rm $(TARGETS)