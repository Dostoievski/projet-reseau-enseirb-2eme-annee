#include<pcap.h> 
#include<stdio.h>
#include<stdlib.h> 
#include<assert.h>
#include<sys/time.h>
#include<unistd.h>

pid_t createProcessus();
FILE* createFile(int i);
pcap_dumper_t* createPcapFile(pcap_t* handle, FILE* f);
