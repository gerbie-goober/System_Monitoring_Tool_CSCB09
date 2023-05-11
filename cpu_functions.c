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

#include "cpu_functions.h"

#define MAX_STR_LEN 2048

/*
  REFER TO .H FILES FOR DOCUMENTATION
*/

void store_cpu_usage(Cpu *sample_cpu){
    FILE *file;
    char line[256] = {0};
    int num[10];
    int total = 0;

    //open /proc/stat, which contains information about the cpu and usage allocation
    if((file = fopen("/proc/stat","r"))){
        //read first line
        fgets(line, sizeof(line), file);
        //split line by space
        char *token = strtok(line, " ");
        if(strcmp(token, "cpu") == 0){
            // loop through the string to extract all other tokens on first line
            int i = 0;
            while((token = strtok(NULL, " ")) != NULL ) {
                //convert string to integer
                int number = strtol(token, NULL, 10);
                //add up all the numbers
                total += number;
                num[i] = number;
                i++;
            }
        }
        sample_cpu->T = total;
        //subtract the idle memory
        sample_cpu->U = total - num[3];
    }
    else{
        perror("/proc/stat could not open properly.\n");
        exit(1);
    }
    fclose(file);
    return;
}

void write_cpu_usage(Cpu *sample_cpu, int fd_write_cpu){
  write(fd_write_cpu, &sample_cpu->U, sizeof(float));
  write(fd_write_cpu, &sample_cpu->T, sizeof(float));
}

int main(int argc, char **argv){
  char fd_write_str[MAX_STR_LEN];
  int fd_write_cpu;

  strncpy(fd_write_str, argv[1], sizeof(fd_write_str));
  fd_write_cpu = atoi(fd_write_str);

  Cpu sample_cpu;
  store_cpu_usage(&sample_cpu);
  write_cpu_usage(&sample_cpu, fd_write_cpu);
  
  return 0;
}