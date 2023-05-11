#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <utmp.h> 

#include "system_functions.h"

#define MAX_STR_LEN 2048

/*
  REFER TO .H FILES FOR DOCUMENTATION
*/

void store_system_usage(Memory *sample_mem){
    struct sysinfo info;
    if(sysinfo(&info)!=0){
      perror("could not access sysinfo!");
      exit(1);
    }
    sample_mem->total_physical = (float)info.totalram/1000000000;
    sample_mem->total_mem = (float)(info.totalram + info.totalswap)/1000000000;
    sample_mem->physical_used = (float)(info.totalram - info.freeram)/1000000000;
    sample_mem->virtual_used =(float)(info.totalram + info.totalswap - info.freeswap - info.freeram)/1000000000;
    return;
}

void write_system_usage(Memory *sample_mem, int fd_write_sys){
  write(fd_write_sys, &sample_mem->physical_used, sizeof(float));
  write(fd_write_sys, &sample_mem->virtual_used, sizeof(float));
  write(fd_write_sys, &sample_mem->total_physical, sizeof(float));
  write(fd_write_sys, &sample_mem->total_mem, sizeof(float));
}

int main(int argc, char **argv){
  char fd_write_str[MAX_STR_LEN];
  int fd_write_sys;

  strncpy(fd_write_str, argv[1], sizeof(fd_write_str));
  fd_write_sys = atoi(fd_write_str);

  Memory sample_mem;
  store_system_usage(&sample_mem);
  write_system_usage(&sample_mem, fd_write_sys);
  
  return 0;
}
