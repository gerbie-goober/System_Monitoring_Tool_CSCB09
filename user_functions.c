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

#include "user_functions.h"

#define MAX_STR_LEN 2048

/*
  REFER TO .H FILES FOR DOCUMENTATION
*/

void write_user_usage(){
  //set pointer to the top of the file
  setutent();
  struct utmp *user_info;
  //iterate through file to get required infomrmation on connected users
  while((user_info = getutent()) != NULL ){
    if(user_info->ut_type == USER_PROCESS){printf("%s       %s(%s)\n", user_info->ut_user, user_info->ut_line, user_info->ut_host);}
  }
  //close session
  endutent();
}

int main(int argc, char **argv){
  
  write_user_usage();
  
  return 0;
}