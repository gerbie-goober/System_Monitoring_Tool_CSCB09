#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utmp.h> 

#include "system_functions.h"
#include "cpu_functions.h"
#include "user_functions.h"

#define MAX_STR_LEN 2048

/*
  Forks and retrieves information about memory utlization in parent and executes system_functions in child
*/
void read_system_usage(Memory *sample_mem){
  int fd[2];
  int pid;

  if(pipe(fd) == -1){
    perror("failed to pipe for system usage");
    exit(1);
  }

  char fd_write_str[MAX_STR_LEN];
  sprintf(fd_write_str, "%d", fd[1]); // store write fd in fd_write_str
  char *args[] = {"./system_functions", fd_write_str, NULL};

  pid = fork();

  if(pid == -1){
    perror("failed to fork for system usage");
    exit(1);
  }

  //child
  if(pid == 0){
    close(fd[0]); // close reading end

    int d = dup2(fd[1], 1);

    if(d == -1){
      perror("failed to duplicate for system usage");
      exit(1);
    }
    if(execv("./system_functions", args) == -1){
      perror("failed to execute for system usage");
    }
  }
  else{
    close(fd[1]); // close writing end

    int read_status;

    read_status = read(fd[0], &sample_mem->physical_used, sizeof(float));
    if(read_status == -1)
    {
        perror("error in trying to read physical_used from child");
        exit(1);
    }

    read_status = read(fd[0], &sample_mem->virtual_used, sizeof(float));
    if(read_status == -1)
    {
        perror("error in trying to read virtual_used from child");
        exit(1);
    }

    read_status = read(fd[0], &sample_mem->total_physical, sizeof(float));
    if(read_status == -1)
    {
        perror("error in trying to read total_physical from child");
        exit(1);
    }

    read_status = read(fd[0], &sample_mem->total_mem, sizeof(float));
    if(read_status == -1)
    {
        perror("error in trying to read total_mem from child");
        exit(1);
    }
  }
  return;
}

/*
  Prints graphics for memory utilization
*/
void print_mem_graphics(Memory sample_mem[], int i){
  if(i == 0){
    printf("   |o 0.00 (%.2f)\n", sample_mem[i].virtual_used);
  }
  else{
    printf("   |");
    float change = sample_mem[i].virtual_used - sample_mem[i-1].virtual_used;
    change = roundf(change * 10)/10;
    int difference = (sample_mem[i].virtual_used - sample_mem[i-1].virtual_used)*100;
    int abs_difference = abs(difference);
    for(int j = 0; j <= abs_difference; j++){
          if(difference > 0){printf("#");}
          if(difference < 0){printf(":");}
    }
    if(difference < 0 && abs_difference < 1){
      printf("@ %.2f (%.2f)\n", change, sample_mem[i].virtual_used);
    }
    else if(difference > 0 && abs_difference < 1){
      printf("o %.2f (%.2f)\n", change, sample_mem[i].virtual_used);
    }
    else{ printf("* %.2f (%.2f)\n", change, sample_mem[i].virtual_used);}        
    }
}

/*
    Prints the utilization of memory that is being done.
    Information reported:
    - Physical RAM memory out of Total Physical memory
    - Virtual memory out of Total Physical memory and Swap Space 
*/
void print_system_usage(Memory sample_mem[], int N, int index, int sequential_true, int graphics_true){
  printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
  //prints N samples
  for(int i = 0; i < N; i++){
      //if --sequential flag entered, then only show memory usage for given iteration
      if(sequential_true){
          if(i == index){ 
            printf("%0.2f GB / %0.2f GB  -- %0.2f GB / %0.2f GB",sample_mem[i].physical_used, sample_mem[i].total_physical, sample_mem[i].virtual_used, sample_mem[i].total_mem);
            if(graphics_true){
              print_mem_graphics(sample_mem, i);
            }
            else{printf("\n");}
          }
          else{printf("\n");}
      } else {
          if(i <= index){ 
            printf("%0.2f GB / %0.2f GB  -- %0.2f GB / %0.2f GB",sample_mem[i].physical_used, sample_mem[i].total_physical, sample_mem[i].virtual_used, sample_mem[i].total_mem);
            if(graphics_true){
              print_mem_graphics(sample_mem, i);
            }
            else{printf("\n");}
            }
          else{printf("\n");}
      }
  }
  printf("---------------------------------------\n");
  return;
}

/*
  Forks and retrieves information about cpu utlization in parent and executes cpu_functions in child
*/
void read_cpu_usage(Cpu *sample_cpu){
  int fd[2];
  int pid;

  if(pipe(fd) == -1){
    perror("failed to pipe for cpu usage");
    exit(1);
  }

  char fd_write_str[MAX_STR_LEN];
  sprintf(fd_write_str, "%d", fd[1]); // store write fd in fd_write_str
  char *args[] = {"./cpu_functions", fd_write_str, NULL};

  pid = fork();

  if(pid == -1){
    perror("failed to fork for cpu usage");
    exit(1);
  }

  //child
  if(pid == 0){
    close(fd[0]); // close reading end

    int d = dup2(fd[1], 1);

    if(d == -1){
      perror("failed to duplicate for cpu usage");
      exit(1);
    }
    if(execv("./cpu_functions", args) == -1){
      perror("failed to execute for cpu usage");
    }
  }
  else{
    close(fd[1]); // close writing end

    int read_status;

    read_status = read(fd[0], &sample_cpu->U, sizeof(float));
    if(read_status == -1)
    {
        perror("error in trying to read U from child");
        exit(1);
    }

    read_status = read(fd[0], &sample_cpu->T, sizeof(float));
    if(read_status == -1)
    {
        perror("error in trying to read T from child");
        exit(1);
    }
  }
  return;
}

/*
  Prints graphics for cpu utilization.
*/
void print_cpu_graphics(Cpu sample_cpu[], int i){
  int bar_numbers = sample_cpu[i].total_cpu_use;
  printf("     |");
  for(int j = 0; j < bar_numbers; j=j+5){
      printf("|");
  }
  printf(" %.2f\n", sample_cpu[i].total_cpu_use);
}

/*
  Prints the CPU usage percentage and number of cores for the machine that is running the program.
*/
void print_core_and_cpu_usage(Cpu sample_cpu[], int num_cores, int N, int index, int seqeuential_true, int graphics_true){
  float temp;
  if(index == 0){
    temp = fabs(sample_cpu[index].U)/(sample_cpu[index].T);
  }
  else{
    temp = fabs(sample_cpu[index].U - sample_cpu[index-1].U)/(sample_cpu[index].T - sample_cpu[index-1].T);
  }
  sample_cpu[index].total_cpu_use = temp*100;
  printf("Number of cores: %d\n   Total cpu use = %.2f%%\n",num_cores,sample_cpu[index].total_cpu_use);
  if(graphics_true){
    for(int i = 0; i < N; i++){
      if(seqeuential_true){
        if(i == index){print_cpu_graphics(sample_cpu, i);}
        else{printf("\n");}
      }
      else{
        if(i <= index){print_cpu_graphics(sample_cpu, i);}
        else{printf("\n");} 
      }
    }
  }
  printf("---------------------------------------\n");
}

/*
  Prints how many users are connected in a given time and how many sessions each user is connected to.
*/
void print_user_usage(){
  int fd[2];
  int pid;
  char users_info[MAX_STR_LEN];

  if(pipe(fd) == -1){
    perror("failed to pipe for user usage");
    exit(1);
  }

  char fd_write_str[MAX_STR_LEN];
  sprintf(fd_write_str, "%d", fd[1]); // store write fd in fd_write_str
  char *args[] = {"./user_functions", NULL};

  pid = fork();

  if(pid == -1){
    perror("failed to fork for user usage");
    exit(1);
  }

  //child
  if(pid == 0){
    close(fd[0]); // close reading end

    int d = dup2(fd[1], 1);

    if(d == -1){
      perror("failed to duplicate for user usage");
      exit(1);
    }
    if(execv("./user_functions", args) == -1){
      perror("failed to execute for user usage");
    }
  }
  else{
    close(fd[1]); // close writing end

    int read_status;

    read_status = read(fd[0], users_info, MAX_STR_LEN);
    if(read_status == -1)
    {
        perror("error in trying to read users string from child");
        exit(1);
    }
    else{
      users_info[read_status] = '\0';
    }
  }

  printf("### Sessions/users ###\n");
  printf("%s", users_info);
  printf("---------------------------------------\n");

  //close session
  endutent();
  return;
}

/*
    Prints the system information of the machine being used to run the program (if successfully access the information using utsname)
    Information reported:
    - System Name
    - Machine Name
    - Version
    - Release
    - Architecture
*/
void print_system_info(){
    struct utsname return_info;
    if(uname(&return_info)==0){
        printf("### System Information ###\nSystem Name = %s\nMachine Name = %s\nVersion = %s\nRelease = %s\nArchitecture = %s\n---------------------------------------\n", return_info.sysname,return_info.nodename, return_info.version, return_info.release, return_info.machine);
    }
    else{printf("Unable to access system information on lab machine.\n");}
    return;
}


/*
  Prompts the user if they want to quit the program if received CTRL+C SIGNAL
*/
void handle_ctrlc(int signum){
  char signal = 'z';
  while(signal != 'y' && signal != 'n'){
    printf("\033c");
    printf("\n\nDo you want to quit the program? (y/n)\n Enter y to confirm.\n");
    scanf("%c", &signal);
  }
  if(signal == 'y'){
    printf("\033c");
    printf("\n\nEXITING PROGRAM.\n");
    exit(0);
  }
}

/*
  Does nothing if received signal
*/
void do_nothing(int signum){return;}

/*  - Manages/watches for signal handling.
    - Prints the section(s) of information according N, T, --user, --system, --graphics if specified/entered. 
    i.e. 
        --samples=N or N: run samples for N iterations
        --tdelay=t or T: for each iteration, wait T seconds
        --user: shows only users and sessions 
        --system: shows system utilization, cpu usage, and number of cores
        --graphics: shows the change in mem. usage and cpu usage from one sample to the one taken before
        o/w: reports all information above
*/
void print_all(int N, int T, int user_true, int system_true, int sequential_true, int graphics_true){
    Memory sample_mem[N];
    Cpu sample_cpu[N];
    int status;
    int num_cores;

    struct rusage mem_usage;
    getrusage(RUSAGE_SELF, &mem_usage);

    //signal handler for CTRL-C
    struct sigaction ctrlc_signal;
    ctrlc_signal.sa_handler = handle_ctrlc;
    ctrlc_signal.sa_flags = 0;

    if(sigaction(SIGINT, &ctrlc_signal, NULL) == -1){
        perror("error with CTRL-C signal");
        exit(1);
    }

    //signal hanlder for CTRL-Z
    struct sigaction ctrlz_signal;
    ctrlz_signal.sa_handler = do_nothing;
    ctrlz_signal.sa_flags = 0;

    if(sigaction(SIGTSTP, &ctrlz_signal, NULL) == -1){
        perror("error with CTRL-Z signal");
        exit(1);
    }

    //signal handler for timer (w/ delay of T seconds)
    struct sigaction timer_signal;
    timer_signal.sa_handler = do_nothing;
    timer_signal.sa_flags = 0;

    struct itimerval timer;
    timer.it_value.tv_sec = T;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = T;
    timer.it_interval.tv_usec = 0;

    if(sigaction(SIGALRM, &timer_signal, NULL) == -1 || setitimer(ITIMER_REAL, &timer, NULL) == -1){
      perror("error with alarm and/or timer");
      exit(1);
    }

    //get num of cores
    num_cores = get_nprocs_conf();

    for(int i = 0; i < N; i++){
      //populate structs with the info from child functions
      read_system_usage(&sample_mem[i]);
      read_cpu_usage(&sample_cpu[i]);

      if(!sequential_true){
        printf("\033c");
      }
      else{
        printf(">>> Iteration: %d\n", i);
      }
      //header: declares how many samples per second will be reported
      printf("Nbr of samples: %d -- every  %d secs\nMemory usage: %li kilobytes\n---------------------------------------\n", N, T, mem_usage.ru_maxrss);

      //defines what will be printed if --system flag given (memory section)
      if(system_true && !user_true){
        print_system_usage(sample_mem, N, i, sequential_true, graphics_true);
        print_core_and_cpu_usage(sample_cpu, num_cores, N, i, sequential_true, graphics_true);
      }

      //defines what will be printed if --user flag given
      if(user_true && !system_true){
          print_user_usage();
      }

      //if no flags are given or if only --sequential flag is given
      else if((user_true && system_true) || (!user_true && !system_true)){
        print_system_usage(sample_mem, N, i, sequential_true, graphics_true);
        print_user_usage();
        print_core_and_cpu_usage(sample_cpu, num_cores, N, i, sequential_true, graphics_true);
      }

      print_system_info();

      pause();
    }

    wait(&status);
    return;
}

int main(int argc, char **argv){
    int N = 10;
    int T = 1;
    int user_true = 0;
    int system_true = 0;
    int sequential_true = 0;
    int graphics_true = 0;

    if (argc == 1){ 
        //if no optional flags are given
        print_all(N, T, 1, 1, 0, 0);
    } 
    else {
        //iterate through the flags given
        for(int i = 1; i < argc; i++){
            //--system flag
            if(strcmp(argv[i], "--system") == 0){system_true = 1;}

            //--user flag
            else if(strcmp(argv[i], "--user") == 0){user_true = 1;}

            //--sequential flag
            else if(strcmp(argv[i], "--sequential") == 0){sequential_true = 1;}

            //--graphics flag
            else if(strcmp(argv[i], "--graphics") == 0){graphics_true = 1;}

            //--samples=N flag
            else if(strstr(argv[i], "--samples=") != NULL){ 
                char *token = strtok(argv[i], "=");
                if((token = strtok(NULL, " ")) != NULL) {N = strtol(token,NULL,10);}
                else {printf("Specify N samples to be taken [positive integer].\n"); return 1;}
            }

            //--tdelay=N flag 
            else if(strstr(argv[i], "--tdelay=") != NULL){
                char *token = strtok(argv[i], "=");
                if((token = strtok(NULL, " ")) != NULL) {N = strtol(token,NULL,10);}
                else {printf("Specify T seconds as the delay for each sample [positive integer].\n"); return 1;}
            } 

            //positional arguments (N || (N && T))
            else if(isdigit(argv[i][0])){
                N = strtol(argv[i], NULL, 10); 
                i++; 
                if(i <= argc - 1){T =  strtol(argv[i], NULL, 10);}
            }
      
            //invalid flag 
            else{
                printf("ERROR: Flag entered not valid.\nTry:\n--user\n--system\n--sequential\n--samples=[N]\n--tdelday=[T]\n");
                return 1;
            }
        }
        print_all(N, T, user_true, system_true, sequential_true, graphics_true);
    }
    return 0;
}