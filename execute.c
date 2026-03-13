#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LOGICAL_CPU_COUNT 8
#define PHYSICAL_CPU_COUNT 4
#define BUFFER_SIZE 64

extern char *commands[];
extern char *matched_command;

char greetings[]= "▀▄   ▄▀  ▀▄   ▄▀  ▀▄   ▄▀  ▀▄   ▄▀\n"
                   " █▄▀█ █▄▀█  █▄▀█ █▄▀█  █▄▀█ █▄▀█\n"
                   " █  █ █  █  █  █ █  █  █  █ █  █\n"
                   "▀   ▀▀   ▀▀   ▀▀   ▀▀   ▀▀   ▀▀   ▀\n"
                   "\n"
                   "  CPU TAMER - Tame your cores, save your battery\n";

int current_cpu_state[LOGICAL_CPU_COUNT];
int new_cpu_state[LOGICAL_CPU_COUNT];


void print_greetings(){
    printf("%s",greetings);
}

int get_current_cpu_state(void){
    for (int i=0;i<LOGICAL_CPU_COUNT;i++){
        current_cpu_state[i]=1;
    }
    return 0;
}

int execute_shell_command(const char *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp("sh", "sh", "-c", cmd, NULL);
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
    return -1;
}

int cpu_set_safety_checker(int* list){//ensure not all cpus are disabled
    int sum=0;
    if(list[0]!=1) return 1;// You cannot shut down cpu0
    for(int i=0;i<LOGICAL_CPU_COUNT;i++){
        sum+=list[i];
    }
    if(sum>0) return 0;//normal
    return 1;//wrong!!! Your CPU will be completely down!!
}

int exec_SMT(char* args){
  if(args==NULL){
    //print current SMT status
    printf("SMT status to be implemented\n");
    return 0;
  }
  if(strcmp(args,"on")==0){
    execute_shell_command("echo on | sudo tee /sys/devices/system/cpu/smt/control");
    return 0;
  }
  if(strcmp(args,"off")==0){
    execute_shell_command("echo off | sudo tee /sys/devices/system/cpu/smt/control");
    return 0;
  }
  return 1;
}

void get_new_cpu_state(int* list, int core, int thread){//assuming intel cpu layout i.e. primary threads cpu0-3, secondary thread cpu4-7
  list[0]=1;//cpu0 must be active
  int active_primary_thread_count=core, active_secondary_thread_count=thread-core;
  for(int i=1;i<LOGICAL_CPU_COUNT;i++){
    list[i]=0;
  }
  for(int i=0;i<active_primary_thread_count;i++){//activate primary threads
    int primary_thread_index=i;
    list[primary_thread_index]=1;
  }
  for(int i=0;i<active_secondary_thread_count;i++){//activate secondary threads
    int secondary_thread_index=i+PHYSICAL_CPU_COUNT;
    list[secondary_thread_index]=1;
  }
}

int exec_cpu_state(int* list){
  if(cpu_set_safety_checker(new_cpu_state)){//again, safety checking
    printf("Illegal cpu state list, cpu state not changed\n");
    return 1;//cpu state list is illegal
  }

  for(int i=1;i<LOGICAL_CPU_COUNT;i++){
    char command[64];
    snprintf(command, sizeof(command), 
          "echo %d | sudo tee /sys/devices/system/cpu/cpu%d/online", 
          list[i], i);
    execute_shell_command(command);
  }
  return 0;
}

int set(int core, int thread){
  get_new_cpu_state(new_cpu_state, core, thread);
  if(exec_cpu_state(new_cpu_state)) return 1;
  return 0;
}

int core_thread_checker(int core, int thread) {
    if (core <= 0 || core > PHYSICAL_CPU_COUNT)
        return 1;               // 核心数非法
    if (thread < core || thread > core * 2)
        return 1;               // 线程数超出允许范围
    return 0;                    // 合法
}

int get_set_args(int* core, int* thread, char* argument){
  if(strcmp(argument, "min")==0){
    *core=1, *thread=2;
    return 0;
  }
  if(strcmp(argument, "max")==0){
    *core=PHYSICAL_CPU_COUNT, *thread=LOGICAL_CPU_COUNT;
    return 0;
  }
  if(sscanf(argument, "%dc%dt",core,thread)==2){
    if(core_thread_checker(*core, *thread)==0) return 0;
  }
  return 1;
}

int handle_first_command(char* first_command,char **cmds){
  if(first_command==NULL){
    return 1;
  }
  if(strcmp(first_command
  , "q")==0){//quit cputamer, return exit code
    return -1;//quit
  }
  if(strcmp(first_command
  , "set")==0){
    int core=PHYSICAL_CPU_COUNT, thread=LOGICAL_CPU_COUNT;
    if(get_set_args(&core, &thread,cmds[1])){
      printf("illegal argument for command: set\n");
      return 2;//error
    }
    set(core, thread);
    return 0;
  }
  if(strcmp(first_command, "smt")==0){
    exec_SMT(cmds[1]);
    return 0;
  }
  return 1;//error
}


