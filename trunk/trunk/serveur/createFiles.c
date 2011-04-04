#include<stdio.h>
#include<stdlib.h> 
#include<assert.h>
#include<sys/time.h>
#include<unistd.h>
#include<pcap.h>
#include"createFiles.h"



pid_t createProcessus(){
  return fork();
}

FILE * createFile(int i){
  char*fname = malloc(sizeof(char) * 26);
  assert(fname);
  sprintf(fname,"File%d",i);
  printf("génération du fichier %s ...", fname);
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

