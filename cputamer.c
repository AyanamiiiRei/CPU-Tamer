#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>//创建进程之类的
#include <sys/types.h> // 用于 pid_t 等类型
#include <sys/wait.h>// 用于 wait() 函数


#define PHYSICAL_CPU_COUNT 4
#define LOGICAL_CPU_COUNT 8
#define BUFFER_SIZE 64
#define COMMAND_COUNT 64

//global variables
char greetings[]= "▀▄   ▄▀  ▀▄   ▄▀  ▀▄   ▄▀  ▀▄   ▄▀\n"
                   " █▄▀█ █▄▀█  █▄▀█ █▄▀█  █▄▀█ █▄▀█\n"
                   " █  █ █  █  █  █ █  █  █  █ █  █\n"
                   "▀   ▀▀   ▀▀   ▀▀   ▀▀   ▀▀   ▀▀   ▀\n"
                   "\n"
                   "  CPU TAMER - Tame your cores, save your battery\n";

int current_cpu_state[LOGICAL_CPU_COUNT], new_cpu_state[LOGICAL_CPU_COUNT];
char buffer[BUFFER_SIZE];
char *commands[COMMAND_COUNT];
int commands_count=0;


/**
  function implementation
**/
void print_greetings(){
    printf("%s",greetings);
}

int get_current_cpu_state(void){
    for (int i=0;i<LOGICAL_CPU_COUNT;i++){
        current_cpu_state[i]=1;
    }
    return 0;
}

int get_input(void){
    printf("CPU-Tamer>");
    if (fgets(buffer,BUFFER_SIZE,stdin)!= NULL){//prevent buffer overflow
        buffer[strcspn(buffer,"\n")]=0;
        return 0;
    }
    return 1;
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

/** defined commands:
2c2t, 2c3t, 2c4t, 3c3t, 3c4t, 3c5t, 3c6t
q, exit--> quit
smt --> show smt status: on/off
"smt on", "smt off"
min --> 1c1t/1c2t
max --> enable all cores and LOGICAL_CPU_COUNT
**/

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
}


int get_commands(){//helper function for parse_input()
      commands_count=0;
     //第一次分割
      char *token = strtok(buffer," \t");//空格 或TAB（制表符）
      //循环获取所有令牌and save them to commands[]    i.e: cpu tamer set 2c4t
      while(token!=NULL && commands_count<COMMAND_COUNT-1){
        commands[commands_count]=token;
        commands_count++;
        token=strtok(NULL," \t");//send NULL is correct, strtok would continue its STATIC POINTER
      }
      commands[commands_count]=NULL;
      if(commands_count==0){
        // printf("No valid command found\n");
        return 1;
      }
      return 0;//parsed all commands and saved to commands[], return 0
}

char *defined_commands[]={"set","q","smt",NULL};
char *matched_command=NULL;

int match_first_cmd(){//save the matched command to **matched_command
  for (int i=0;;i++){
  if(defined_commands[i]==NULL){
    break;
  }
  if(strcmp(commands[0],defined_commands[i]) == 0){
    matched_command=defined_commands[i];//存储匹配的内部命令指针
    return 0;//matched
  }
  }
  return 1;//nothing matched
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

int handle_first_command(char* first_command,char **commands){
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
    if(get_set_args(&core, &thread,commands[1])){
      printf("illegal argument for command: set\n");
      return 2;//error
    }
    set(core, thread);
    return 0;
  }
  if(strcmp(first_command, "smt")==0){
    exec_SMT(commands[1]);
    return 0;
  }
  return 1;//error
}

int parse_input(){
    if(get_commands()) {
      printf("No valid command found\n");
      return 1;
    }
    //matching first command i.e: cd,pwd 
    if(match_first_cmd()){
      printf("%s: Not a command\n",commands[0]);
      return 1;
    }
    return 0;
}



int main(){
  print_greetings();
  while(1){
  if(get_input()){
    printf("Input buffer overflow\n");
    continue;
  }
  if(parse_input()){
    printf("parse_input() failed\n");
    continue;
  }
  switch (handle_first_command(matched_command,commands)){
    case -1:
      printf("Exiting...\n");
      return 0;
    case 1:
      printf("No command executed\n");
      continue;
    case 2:
      continue;
    default:
      ;
  }
  // printf("\n");
  }
}


