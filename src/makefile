IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR) -lm
# -lm is needed to include sin and cos from math.h. C is annoying

ODIR=obj
LDIR =../lib

_DEPS = feedback.h whisper.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o feedback.o whisper.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 