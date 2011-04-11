#include<stdio.h>
#include<stdlib.h> 
#include<assert.h>
#include<sys/time.h>
#include<unistd.h>
#include<pcap.h>
#include"time.h"
#include "math.h"
double get_time() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return (tv.tv_sec + tv.tv_usec/1000000.0);
}

int one_minute_past(double t1, double t2){
  t2=get_time();
  //t2=get_time();
  //printf("%g t1-t2\n",t1-t2);
  if(abs(t1-t2)>60.0){
    printf("one minute is past\n");
    return 0;
  }
  return 1;
}
