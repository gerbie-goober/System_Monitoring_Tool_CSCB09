# **SYSTEM MONITORING TOOL**
### **UPDATED**: can run with --graphics flag and program runs concurrently
#### Contributor: _Matthew Wu (1008471798)_

## _**Project Description**_
> * This project uses C to display various information (i.e. user usage, system usage, system information) about the machine (a Linux/Unix system). 
> * Includes signal handling for: CTRL+C (quit program) AND CTRL+Z (do nothing -- program is not interrupted)


## _**How the Project was Approached**_
> To display each section of information, processes were run concurrently to find each query of information. That is, the main process forks each time per sample to communicate with child functions to retrieve the desired information (e.g. physical/virtual memory used, cpu samples, etc.) Sub-functions (e.g. separate c files) would be responsible for finding information on memory utilization, connected users, and CPU utilization. These programs would write back the information to the main program/process, which would read it and print the said information. 

## _**Overview of Functions**_
> #### _**User_Functions**:_
> * #### **write_user_usage**: prints to stdout the users that are currently on the system (stdout is then captured in parent function)
>   * _INPUT(S)_: `none`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `utmp.h`
> #### _**System_Functions**:_
> * ####  **store_system_usage**: function that calculates and stores the Physical RAM / Total Physical and Virtual Mem / Total Physical + Swap Space for a given sample to be taken during a given time
>   * _INPUT(S)_: `Memory *sample_mem`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `sys/sysinfo.h`
> * ####  **write_system_usage**: function that writes the given information that was acquired and stored in `store_system_usage` with the given FD
>   * _INPUT(S)_: `Memory *sample_mem, int fd_write_sys`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `none`
> * ####  **read_system_usage** (found in main_program, `print.c`)**: function that executes the child cpu function and reads in parent the information written for memory utilization
>   * _INPUT(S)_: `Memory *sample_mem`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `none`
> #### _**CPU_Functions**:_
> * ####  **store_cpu_usage**: function that calculates and stores total cpu usage time from all cores
>   * _INPUT(S)_: `Cpu *sample_cpu`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `none`
> * ####  **write_cpu_usage**: function that writes the given information that was acquired and stored in `store_cpu_usage` with the given FD
>   * _INPUT(S)_: `Cpu *sample_mem, int fd_write_cpu`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `none`
> * ####  **read_cpu_usage (found in main_program, `print.c`)**: function that executes the child cpu function and reads in parent the information written for cpu usage
>   * _INPUT(S)_: `Cpu *sample_cpu`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `none`
> #### _**Print_Functions (found in main_program, `print.c`)**:_
> * #### **print_user_usage**: function that prints the number users connected to machine in a given time and how many sessions the users are connected to (accomplishes this by executing child user program and reads stdout using dup2)
>   * _INPUT(S)_: `none`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `none`
> * ####  **print_system_usage**: function that prints the Physical RAM / Total Physical and Virtual Mem / Total Physical + Swap Space for a given N samples to be taken during a given time
>   * ####   if `--sequential` flag is entered, only print given memory utilization for a given iteration --> else, print current iteration and previous iterations of sampling
>   * ####   if `--graphics` flag is entered, print `:` for each 0.1 negative change from previous sample to current and `#` for each 0.1 positive change from previous sample to current
>   * _INPUT(S)_: `Memory sample_mem[], int N, int index, int sequential_true, int graphics_true`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `none`
>   * HELPER FUNC.: `void print_mem_graphics(Memory sample_mem[], int i)`: prints graphics as described above
> * #### **print_core_and_cpu_usage**: function that prints the number of cores for a given machine and the cpu utlization during each sample taken
>   * cpu_utilization defined as how much cpu worked in one instance of time and the next
>     * calculated as taking one sample of cpu (totalling all cpu usage) and taking another sample of cpu --> [|(T1- idle1) - (T2 - idle2)| / (T1 - T2)]*100 % 
>   * ####   if `--sequential` flag is entered, only print given cpu utilization for a given iteration --> else, print current iteration and previous iterations of sampling
>   * ####   if `--graphics` flag is entered, print `|` for each 5% change from previous sample to current
>   * _INPUT(S)_: `Cpu sample_cpu[], int num_cores, int N, int index, int seqeuential_true, int graphics_true`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `math.h`
>   * HELPER FUNC.: `void print_cpu_graphics(Cpu sample_cpu[], int i)`: prints graphics as described above
> * #### **print_all**: print user usage, system usage, or both depending on the flags entered; leverages timer as way to set delay between each sample
>   * _INPUT(S)_: `int N, int T, int user_true, int system_true, int sequential_true, int graphics_true`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `sys/resource.h`; `signal.h`, `sys/resource.h`, `sys/types.h`, `sys/wait.h`
>   * HELPER FUNC.: `void handle_ctrlc(int signum)` and `void do_nothing(int signum)`: signal handling for CTRL+C, CTRL+Z, and timer
> * #### **print_system_info**: print System Name, Machine Name, Version, Release, and Architecture
>   * _INPUT(S)_: `none`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `sys/utsname.h`
> #### _**Main (found in `print.c`)**:_
> * #### parse through all flags and arguments, call `print_all` and report information properly
>   * _INPUT(S)_: `int argc, char** argv`
>   * _RETURN_: `int`
>   * _ADDITIONAL LIBRARIES USED_: `ctype.h`

## _**How To Run the Program**_
> ### _**COMPILING**_: 
> ### `make`
> ### _**RUNNING**_: 
> ### `./print [...]` <-  put flags here
> ### _**VALID FLAGS**_: 
> * `--user` : only display user usage of system
> * `--system`: only display system usage of system
> * `--sequential`: display information sequentially without "refreshing" the screen 
> * `--graphics`: display additional graphics for change in memory utilization and cpu usage from previous sample to current  
> * `--samples=N`; N is a parameter that you must specify (a positive integer) [default is 10]
> * `--tdelay=T`; T is a parameter that you must specify (a positive integer) [default is 1]