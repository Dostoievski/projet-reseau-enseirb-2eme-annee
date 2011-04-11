CC      = gcc
CFLAGS  =
LDFLAGS =

PROG  = serveur

OBJS  = programme.o createFiles.o time.o

all: $(PROG)

time.o:time.c time.h
	$(CC) -c time.c

createFiles.o :createFiles.c createFiles.h
	$(CC) -c createFiles.c

programme.o : programme.c programme.h createFiles.h time.h
	$(CC) -c `pkg-config  --cflags glib-2.0` programme.c	
$(PROG): $(OBJS)
	$(CC) -lpthread -lpcap -o $(PROG) $(OBJS) `pkg-config --libs glib-2.0`

clean :
	\rm -f $(PROG) *.o *~ log.pcap File*

