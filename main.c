#include <stdio.h>
#include <unistd.h>//创建进程之类的
#include <sys/types.h> // 用于 pid_t 等类型
#include <sys/wait.h>// 用于 wait() 函数


extern void print_greetings(void);
extern int get_current_cpu_state(void);
extern int get_input(void);
extern int parse_input(void);
extern int handle_first_command(char* first_command, char **commands);
extern char *matched_command;
extern char *commands[];

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
  }
}


