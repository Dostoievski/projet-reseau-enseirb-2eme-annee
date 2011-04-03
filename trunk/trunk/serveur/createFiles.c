#include<stdio.h>
#include<stdlib.h> 
#include<assert.h>
#include<sys/time.h>
#include<unistd.h>
#include<pcap.h>
#include"createFiles.h"



pid_t createProcessus(){
  pid_t p = fork();
  return p;
}

FILE * createFile(int i){
  char*fname = malloc(sizeof(char));
  assert(fname);
  sprintf(fname,"File%d",i);
  FILE*f = fopen(fname,"wb");
  if(f==NULL){
    printf("Unable to create file.\n");
    exit(1);
  }
  return f;
}

pcap_dumper_t* createPcapFile(pcap_t* handle, FILE* f){
  pcap_dumper_t *dumpdesc= pcap_dump_fopen(handle,f);
  if(dumpdesc == NULL){
    printf("ERROR dump\n");
    exit(1);
  }
  return dumpdesc;
}

