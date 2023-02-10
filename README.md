# **System Monitoring Tool**
#### Contributor: _Matthew Wu (1008471798)_

## _**Project Description**_
> This project uses C to display various information (i.e. user usage, system usage, system information) about the machine operating on Linux and running the program.


## _**How the Project was Approached**_
> To display each section of information, functions were desgined in a modular way. There is a function that manages which sections of information to be reported given a flag/flags that were entered. Sub-functions would be reponsible for printing strictly the User Usage section, System Usage section, CPU usage and Core section, and System Information section.

## _**Overview of Functions**_
> #### _**User Usage**:_
> * #### **print_user_usage**: function that printed the number users connected to machine in a given time and how many sessions the users are connected to
>   * _INPUT(S)_: `none`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `utmp.h`
> #### _**System Usage**:_
> * ####  **print_system_usage**: function that printed the Physical RAM / Total Physical and Virtual Mem / Total Physical + Swap Space for a given N samples to be taken during a given time
>   * ####   if `--sequential` flag is entered, only print given memory utilization for a given iteration --> else, print current iteration and previous iterations of sampling
>   * _INPUT(S)_: `struct sysinfo *storage_info, int N, int T, int sequential_true`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `sys/sysinfo.h`
> #### _**CPU Usage and # of Cores**:_
> * #### **find_cores**: find number of cores for machine
>   * _INPUTS_: `none`
>   * _RETURN_: `int`
>   * _ADDITIONAL LIBRARIES USED_: accessed `/proc/stat`; `string.h`
> * #### **take_sample**: take sample of memory utilization for a given time
>   * _INPUT(S)_: `none`
>   * _RETURN_: `int`
>   * _ADDITIONAL LIBRARIES USED_: accessed `/proc/stat`; `string.h`
> * #### **print_core_and_cpu_usage**: function that printed the number users connected to machine in a given time and how many sessions the users are connected to
>   * cpu_usage defined as how much cpu worked in one instance of time and the next
>     * calculated as taking one sample of cpu (totalling all cpu usage) and taking another sample of cpu --> (|SAMPLE2 - SAMPLE1| / SAMPLE1)*100 % 
>   * _INPUT(S)_: `none`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `math.h`
> #### _**Print All Sections** (according to --user and --system flags):_
> * #### **print_all**: print user usage, system usage, or both depending on the flags entered
>   * _INPUT(S)_: `int N, int T, int user_true, int system_true, int index, int sequential_true`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `sys/sysinfo.h`; `sys/resource.h` 
> #### _**Print System Information**:_
> * #### **print_system_info**: print System Name, Machine Name, Version, Release, and Architecture
>   * _INPUT(S)_: `none`
>   * _RETURN_: `none`
>   * _ADDITIONAL LIBRARIES USED_: `sys/utsname.h`
> #### _**Main**:_
> * #### parse through all flags and arguments, call print_all and report information properly if given `--sequential` flag or not
>   * _INPUT(S)_: `int argc, char** argv`
>   * _RETURN_: `int`
>   * _ADDITIONAL LIBRARIES USED_: `ctype.h`

## _**How To Run the Program**_
> ### _**COMPILING**_: 
> ### `gcc -Wall mySystemStats.c -o [name of exec.].out`
> ### _**RUNNING**_: 
> ### `./[name of exec. file].out [...]` <-  put flags here
> ### _**VALID FLAGS**_: 
> * `--user` : only display user usage of system
> * `--system`: only display system usage of system
> * `--sequential`: display information sequentially without "refreshing" the screen   
> * `--samples=N`; N is a parameter that you must specify (a positive integer) [default is 10]
> * `--tdelay=T`; T is a parameter that you must specify (a positive integer) [default is 1]