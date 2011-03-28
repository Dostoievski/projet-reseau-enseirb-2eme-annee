#
#
#

CC = gcc
#CFLAGS = -g -v
CFLAGS = -g
LDFLAGS = -g
LDLIBS =  -lnsl

EXE = client server


all : $(EXE)

client : client.o common.o
	$(CC) $(LDFLAGS) client.o common.o $(LOADLIBES) $(LDLIBS) -o $@
server : server.o common.o
	$(CC) $(LDFLAGS) server.o common.o $(LOADLIBES) $(LDLIBS) -o $@


client.o : client.c requests.h common.h
server.o : server.c requests.h common.h
common.o : common.c common.h

clean:
	-rm -f $(EXE) *.o *~
