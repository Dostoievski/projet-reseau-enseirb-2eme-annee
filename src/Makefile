

CC = gcc
#CFLAGS = -g -v
CFLAGS = -g
LDFLAGS = -g
LDLIBS =  -lnsl -lpthread -lpcap

EXE = client server


all : $(EXE)

client : client.o common.o process_client.o
	$(CC) $(LDFLAGS) client.o common.o process_client.o $(LOADLIBES) $(LDLIBS) -o $@
server : server.o common.o sniffer.o process_server.o
	$(CC) $(LDFLAGS) server.o common.o sniffer.o process_server.o $(LOADLIBES) $(LDLIBS) -o $@


client.o : client.c requests.h common.h process_client.h
server.o : server.c requests.h common.h sniffer.h
common.o : common.c common.h
sniffer.o : sniffer.c sniffer.h process_server.h
process_server.o : process_server.c process_server.h requests.h
process_client.o : process_client.c process_client.h requests.h

clean:
	-rm -f $(EXE) *.o*  *#  *~ log
