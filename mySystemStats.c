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

/**
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

/**
    Prints the utilization of memory that is being done.
    Information reported:
    - Physical RAM memory out of Total Physical memory
    - Virtual memory out of Total Physical memory and Swap Space 
*/
void print_system_usage(struct sysinfo *storage_info, int N, int index, int sequential_true){
    struct sysinfo info = *storage_info;
    if(sysinfo(&info)==0){
        printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
        //prints N samples
        for(int i = 0; i < N; i++){
            //if --sequential flag entered, then only show memory usage for given iteration
            if(sequential_true){
                if(i == index){ printf("%0.2f GB / %0.2f GB  -- %0.2f GB / %0.2f GB\n",(float)(info.totalram - info.freeram)/1000000000, (float)info.totalram/1000000000, (float)(info.totalram + info.totalswap - info.freeswap - info.freeram)/1000000000, (float)(info.totalram + info.totalswap)/1000000000);}
                else {printf("\n");}
            } else {
                if(i <= index){ printf("%0.2f GB / %0.2f GB  -- %0.2f GB / %0.2f GB\n",(float)(info.totalram - info.freeram)/1000000000, (float)info.totalram/1000000000, (float)(info.totalram + info.totalswap - info.freeswap - info.freeram)/1000000000, (float)(info.totalram + info.totalswap)/1000000000);}
                else {printf("\n");}
            }
        }
        printf("---------------------------------------\n");
    }
    else{
        printf("Could not access sysinfo.\n");
    }
    return;
}

/**
    Prints how many users are connected in a given time and how many sessions each user is connected to.
*/
void print_user_usage(){
    //set pointer to the top of the file
    setutent();
    struct utmp *user_info;
    printf("### Sessions/users ###\n");

    //iterate through file to get required infomrmation on connected users
    while((user_info = getutent()) != NULL ){
        if(user_info->ut_type == USER_PROCESS){printf("%s       %s (%s)\n",user_info->ut_user,user_info->ut_line, user_info->ut_host);}
    }
    printf("---------------------------------------\n");

    //close session
    endutent();
    return;
}

/**
    Takes sample of memory utilization.
*/
int take_sample(){
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
        //subtract the idle memory
        total = total - num[3];
    }
    else{
        printf("/proc/stat could not open properly.\n");
        return 0;
    }
    fclose(file);
    return total;
}

/**
    Calculates the number of cores for the machine that is running the program.
*/
int find_num_cores(){
    FILE *file;
    char line[256] = {0};
    int num_cores = 0;

    //open /proc/stat, which contains information about the cpu and usage allocation
    if((file = fopen("/proc/stat","r"))){
        //read first line
        fgets(line, sizeof(line), file);
        while(fgets(line, sizeof(line), file)){
            char *token = strtok(line, " ");
            //read scecond line and onwards until cpu[N] is no longer present 
            if(strstr(token, "cpu") != NULL){num_cores++;}
            else{break;}
        }
    }
    else{
        printf("/proc/stat could not open properly.\n");
        return 0;
    }
    fclose(file);
    return num_cores;

}

/**
    Prints the CPU usage percentage and number of cores for the machine that is running the program.
*/
void print_core_and_cpu_usage(){
    int sample_1;
    int sample_2;
    float cpu_use;
    int num_cores;

    //take two samples: SAMPLE 1 (previous) and SAMPLE 2 (current)
    sample_1 = take_sample();
    sample_2 = take_sample();

    //calculate cpu_usage with the |SAMPLE 2 - SAMPLE 1| / SAMPLE 1
    cpu_use = fabs((sample_2 - sample_1)/sample_1)*100;
    
    //find number of cores for machine
    num_cores = find_num_cores();

    printf("Number of cores: %d\n total cpu use = %.2f%%\n---------------------------------------\n",num_cores, cpu_use);

}

/**
    Prints the section(s) of information according N, T, --user, --system if specified/entered.
    i.e. 
        --samples=N or N: run samples for N iterations
        --tdelay=t or T: for each iteration, wait T seconds
        --user: shows only users and sessions 
        --system: shows system utilization, cpu usage, and number of cores
        o/w: reports all information above
*/
void print_all(int N, int T, int user_true, int system_true, int index, int sequential_true){
    struct sysinfo storage_info;
    struct rusage mem_usage;
    getrusage(RUSAGE_SELF, &mem_usage);

    //header: declares how many samples per second will be reported
    if(sysinfo(&storage_info)==0){
        printf("Nbr of samples: %d -- every  %d secs\nMemory usage: %li kilobytes\n---------------------------------------\n", N, T, mem_usage.ru_maxrss);
    }

    //defines what will be printed if --user flag given
    if(user_true){
        print_user_usage();
        return;
    }

    //defines what will be printed if --system flag given
    if(system_true){
        print_system_usage(&storage_info, N, index, sequential_true);
        print_core_and_cpu_usage();
        return;
    }

    else {
        print_system_usage(&storage_info, N, index, sequential_true);
        print_user_usage();
        print_core_and_cpu_usage();
    }
    return;
}


int main(int argc, char **argv){
    int N = 10;
    int T = 1;
    int user_true = 0;
    int system_true = 0;
    int sequential_true = 0;

    if (argc == 1){ 
        //if no optional flags are given
        for(int i = 0; i < N; i++){
            system("clear");
            print_all(N, T, user_true, system_true, i, sequential_true);
            sleep(T);
        }
        sleep(T);
        print_system_info();
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

        if(sequential_true){
            //if sequential flag given, then don't clear screen
            for(int i = 0; i < N; i++){
                printf(">>> Iteratation: %d\n", i);
                print_all(N, T, user_true, system_true, i, sequential_true);
                sleep(T);
            } 
            print_system_info();
        }
        else{
            for(int i = 0; i < N; i++){
                system("clear");
                print_all(N, T, user_true, system_true, i, sequential_true);
                sleep(T);
            }
            print_system_info();
        }
    }
    return 0;
}