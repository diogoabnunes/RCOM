CC = gcc
CFLAGS = -Wall
DEPS = 
OBJ = data_link.c
TARGETS = app

all: app

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

app: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $@.c $(OBJ)

clean:
	@rm $(TARGETS)

# gcc app.c data_link.c -o app