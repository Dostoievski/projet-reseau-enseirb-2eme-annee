#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <pcap.h>
#include <string.h>
#include "sniffer.h"
#include "requests.h"
#include <ctype.h>

void look_for_pswd (char * text,char * delimiter_){
  char *str1 = malloc(strlen(text)*sizeof(char));
  
  char * delimiter = delimiter_;

  char *token;
  char *saveptr1,*saveptr2;
  

  for (strcpy(str1,text); ; str1 = NULL) {
    token = strtok_r(str1,delimiter, &saveptr1);
    if (token == NULL)
      break;
    else if (strcmp(token,"pswd") ==0)
      
      {
	token = strtok_r(saveptr1, delimiter, &saveptr2);
	if (strcmp(token,"=")==0)  
	  printf(" %s\n",strtok(saveptr2,delimiter)  );
      }
    else continue;
  }
  free(str1);
} 

int main (){
 char * text = "mehdi jabri est un pd qui  a laissé son pswd = hmar !!";
 look_for_pswd(text," ");
 int fd = fopen("log", "w+");
 fprintf(fd,"%s\n",text);
 fprintf(fd," achraf zamel\n");
 

}
