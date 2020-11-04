CC = gcc
CFLAGS = -Wall
DEPS = defines.h data_link.h state_machine.h utils.h app_emissor.h app_recetor.h
OBJ = data_link.c state_machine.c utils.c app_emissor.c app_recetor.c
TARGETS = app

all: app

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

app: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ)

clean:
	@rm $(TARGETS)