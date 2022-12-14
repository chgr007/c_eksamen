# TODO: Fjerne debug

INCLDIR = ./include
CC = gcc
CFLAGS = -O2
CFLAGS += -I$(INCLDIR)
LINKEXTRA = -lpthread
LDFLAGS = -Wall -Wextra -g -O0 -I$(INCLDIR)
#LDFLAGS += -ansi -pedantic
OBJDIR = obj
DEBUGOBJS = debugobjs

_DEPS = main.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = main.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(LDFLAGS)


oppgave4: $(OBJS)
	gcc -o $@ $^ $(LDFLAGS) $(LINKEXTRA)


.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o
