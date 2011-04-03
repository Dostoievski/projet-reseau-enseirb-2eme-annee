#include<stdio.h>
#include<stdlib.h> 
#include<assert.h>
#include<sys/time.h>
#include<unistd.h>
#include<pcap.h>
#include"time.h"

float get_time() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return 60.0*(tv.tv_sec + tv.tv_usec*1000.0);
}

int one_minute_past(float t1, float t2){
  t1=get_time();
  t2=get_time();
  if((t1-t2)>1){
    printf("one minute is past\n");
    return 0;
  }
  return 1;
}
