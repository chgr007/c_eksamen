INCLDIR = ./include
CC = gcc
CFLAGS = -O2
CFLAGS += -I$(INCLDIR)
DEBUGFLAGS = -Wall -Wextra -ansi -pedantic -g -O0 -I$(INCLDIR)
OBJDIR = obj

_DEPS = main.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = main.o linked_list.o booking_system.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

oppgave3: $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

debug: $(OBJS)
	$(CC) -o $@ $^ $(DEBUGFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o
