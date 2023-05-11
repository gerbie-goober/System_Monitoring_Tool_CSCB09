#include <stdio.h>

#ifndef _CPU_FUNCTIONS_H
#define _CPU_FUNCTIONS_H

typedef struct cpu_usage {
  float U;
  float T;
  float total_cpu_use;
} Cpu;

/*
  stores total cpu utilization for given sample along with the total cpu time - idle into Cpu struct
*/
void store_cpu_usage(Cpu *sample_cpu);

/*
  writes the information stored into the Cpu at the given FD
*/
void write_cpu_usage(Cpu *sample_mem, int fd_write_cpu);
#endif