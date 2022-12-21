# TODO: Fjerne debug

INCLDIR = ./include
CC = gcc
CFLAGS = -O2
CFLAGS += -I$(INCLDIR)
LINKEXTRA = -lpthread
LDFLAGS = -Wall -Wextra -g -O0 -I$(INCLDIR)
#LDFLAGS += -ansi -pedantic
OBJDIR = obj
DEBUGOBJSDIR = debugobjs

_DEPS = pdf_reader.h
DEPS = $(patsubst %,$(INCLDIR)/%,$(_DEPS))

_OBJS = main.o pdf_reader.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

_DEBUGOBJS = main.o
DEBUGOBJS = $(patsubst %,$(DEBUGOBJSDIR)/%,$(_DEBUGOBJS))


$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(DEBUGOBJSDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(LDFLAGS) -DDEBUG

oppgave5: $(OBJS)
	gcc -o $@ $^ $(CFLAGS) $(LINKEXTRA)

debug: $(DEBUGOBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LINKEXTRA)


.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o
