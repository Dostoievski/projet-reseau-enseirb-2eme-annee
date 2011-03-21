CC      = gcc
CFLAGS  =
LDFLAGS =

PROG  = serveur

OBJS  = programme.o 

all: $(PROG)

programme.o : programme.c programme.h
	$(CC) -c `pkg-config --cflags glib-2.0` programme.c	


$(PROG): $(OBJS)
	$(CC) -lpcap -o $(PROG) programme.o `pkg-config --libs glib-2.0`

clean :
	\rm -f $(PROG) *.o *~

