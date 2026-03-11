#include <ctype.h>
#include <stdio.h>
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
int get_current_cpu_state(void){
    for (int i=0;i<LOGICAL_CPU_COUNT;i++){
        current_cpu_state[i]=1;
    }
    return 0;
}

int get_input(void){
    printf("CPU-Tamer>$");
    if (fgets(buffer,BUFFER_SIZE,stdin)!= NULL){//prevent buffer overflow
        buffer[strcspn(buffer,"\n")]=0;
        return 0;
    }
    return 1;
}

int cpu_exec_set_safety_checker(int* list){//ensure not all cpus are disabled
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
  if(strcmp(args,NULL)==0){
    //print current SMT status
    printf("SMT status to be implemented");
    return 0;
  }
  if(strcmp(args,"on")){
    execute_shell_command("echo on | sudo tee /sys/devices/system/cpu/smt/control");
  }
  if(strcmp(args,"off")){
    execute_shell_command("echo off | sudo tee /sys/devices/system/cpu/smt/control");
  }
}


int get_commands(){//helper function for parse_input()
      commands_count=0;
     //第一次分割
      char *token = strtok(buffer," \t");//空格 或TAB（制表符）
      //循环获取所有令牌and save them to commands[]    i.e: cpu tamer exec_set 2c4t
      while(token!=NULL && commands_count<COMMAND_COUNT-1){
        commands[commands_count]=token;
        commands_count++;
        token=strtok(NULL," \t");//send NULL is correct, strtok would continue its STATIC POINTER
      }
      commands[commands_count]=NULL;
      if(commands_count==0){
        // printf("No valid command found\n");
        return -1;
      }
      return 0;//parsed all commands and saved to commands[], return 0
}

char *cmdList[]={"exec_set","q","smt",NULL};
char *matched_first_cmd=NULL;

int match_first_cmd(){//save the first matched command to **matched_first_cmd**
  for (int i=0;cmdList[i]!=NULL;i++){
  if(strcmp(commands[0],cmdList[i]) == 0){
    matched_first_cmd=cmdList[i];//存储匹配的内部命令指针
    return 0;//matched
  }
  }
  return -1;
}

int core_thread_checker(int core, int thread) {
    if (core <= 0 || core > PHYSICAL_CPU_COUNT)
        return 1;               // 核心数非法
    if (thread < core || thread > core * 2)
        return 1;               // 线程数超出允许范围
    return 0;                    // 合法
}


int get_exec_set_args(int* core, int* thread){
  if(strcmp(cmdList[1], "min")==0){
    *core=1, *thread=2;
    return 0;
  }
  if(strcmp(cmdList[1], "max")==0){
    *core=PHYSICAL_CPU_COUNT, *thread=LOGICAL_CPU_COUNT;
    return 0;
  }
  if(sscanf(cmdList[1], "%dc%dt",core,thread)==2){
    if(core_thread_checker(*core, *thread)) return 0;
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
  if(cpu_exec_set_safety_checker(new_cpu_state)){//again, safety checking
    printf("Illegal cpu state list, cpu state not changed");
    return 1;//cpu state list is illegal
  }

  for(int i=0;i<LOGICAL_CPU_COUNT;i++){
    char command[64];
    snprintf(command, sizeof(command), 
          "echo %d | sudo tee /sys/devices/system/cpu/cpu%d/online", 
          list[i], i);
    execute_shell_command(command);
  }
  return 0;
}

int exec_set(int core, int thread){
  get_new_cpu_state(new_cpu_state, core, thread);
  if(exec_cpu_state(new_cpu_state)) return 1;
  return 0;
}

int handle_first_cmd(){
  if(strcmp(matched_first_cmd, "q")==0){//quit cputamer, return exit code
    return -1;
  }
  if(strcmp(matched_first_cmd, "exec_set")==0){
    int core=PHYSICAL_CPU_COUNT, thread=LOGICAL_CPU_COUNT;
    if(get_exec_set_args(&core, &thread)){
      printf("illegal argument for command: exec_set");
      return 1;//error
    }
    exec_set(core, thread);
  }
  if(strcmp(matched_first_cmd, "smt")==0){
    exec_SMT(cmdList[1]);
  }

}

int parse_input(){
    if(get_commands()) {
      printf("No valid command found\n");
      return 1;
    }
    //matching first command i.e: cd,pwd 
    if(matched_first_cmd){
      printf("%s: Not a command",commands[0]);
      return 1;
    }
    //now matched first command
    *matched_first_cmd
}



void print_greetings(){
    printf("%s",greetings);
}




//main()
int main(){
  // exec_setenv("PATH", "/bin:/usr/bin:/usr/local/bin", 1);
  // char buffer[256];
  //我们的代码
  while (1) {
    //获取当前工作目录
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));
    printf("skublash%s>$ ",cwd);//打印当前工作目录          ！假设：正常情况下cwd不可能返回NULL
    // 2. 读取用户输入
    // fgets 会从“标准输入”（stdin，通常是键盘）读取一行文本，并将其存入 buffer 数组。
    // sizeof(buffer) 确保了读取不会超过数组的容量，防止溢出，这是安全的关键。
    if (fgets(buffer,sizeof(buffer),stdin)!= NULL){
    //3. 回显用户输入
      printf("You entered: %s%s",buffer,"processing command...\n");
      // --- 2. 解析输入：去除末尾的换行符 '\n' ---
            // buffer 现在是 "ls\n\0"，我们需要把它变成 "ls\0\0"
            // 思路：找到换行符，并用字符串结束符 '\0' 覆盖它。
      buffer[strcspn(buffer,"\n")]='\0';
      
      // --- 3. 准备参数列表 for execvp ---
            // execvp 需要一个参数数组，例如执行 "ls -l" 需要数组： {"ls", "-l", NULL}
            // 我们现在先简单处理：把整个输入作为一个命令，参数列表就是 {buffer, NULL}
      char *commands[64];// c语言没有动态的集合 不能声明空集合 更不能往集合里面append或者add元素
      int commands_count=0;
      //第一次分割
      char *token = strtok(buffer," \t");
      //循环获取所有令牌
      while(token!=NULL && commands_count<63){
        commands[commands_count]=token;
        commands_count++;
        token=strtok(NULL," \t");
      } 
      commands[commands_count]=NULL;
      if(commands_count==0){continue;}
      //检查是否是内部命令 例如cd,pwd 
      char *cmdList[]={"cd","exit",NULL};
      char *matched_first_cmd=NULL;

      for (int i=0;cmdList[i]!=NULL;i++)//内部命令匹配
      {
        if(strcmp(commands[0],cmdList[i]) == 0){
          matched_first_cmd=cmdList[i];//存储匹配的内部命令指针
          break;
        }
      }

      if(matched_first_cmd!=NULL){
        printf("DEBUG: Built-in Command Matched!!! \nTrying to execute: %s\n", commands[0]);
        if (strcmp(matched_first_cmd,"cd")==0){
          chdir(commands[1]);
        }
        if(strcmp(matched_first_cmd,"exit")==0){
          printf("Goodbye!\n");
        break;
        }
        continue;
      }

      // --- 4. 外部命令
      //创建子进程并执行命令 ---
      pid_t pid=fork();
      if(pid==-1){
        perror("fork failed");
        continue;//skip this loop and enter the next loop
      } else if (pid==0) {
        // 这里是子进程
        // if (strcmp(commands[0],"pwd")==0){
        //   printf("%s",cwd);
        // }


                // 使用 execvp 来执行命令。如果执行成功，这行代码以下的代码都不会运行。
        printf("DEBUG: Trying to execute: %s\n", commands[0]);
        // printf("DEBUG: PATH=%s\n", getenv("PATH"));
        execvp(commands[0],commands);
        //
        perror("execvp failed");
        return 1;//子进程异常退出
          
      } else{
        // 这里是父进程 (pid > 0)
                // 等待子进程执行完毕
        wait(NULL);
      }
    }
     else {//如果返回了NULL就退出循环
        printf("Exiting...\n");
        break;
