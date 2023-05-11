#include <stdio.h>

#ifndef _SYSTEM_FUNCTIONS_H
#define _SYSTEM_FUNCTIONS_H

typedef struct memory {
  float physical_used;
  float virtual_used;
  float total_physical;
  float total_mem;
} Memory;

/*
  stores the physical, virutal memory used, total physical memory, and total memory including swap into Memory struct
*/
void store_system_usage(Memory *sample_mem);

/*
  writes the information stored in Memory struct at the given FD
*/
void write_system_usage(Memory *sample_mem, int fd_write_sys);

#endif