#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>//创建进程之类的
#include <sys/types.h> // 用于 pid_t 等类型
#include <sys/wait.h>// 用于 wait() 函数


#define CPU_CORES 4
#define THREADS 8
#define BUFFER_SIZE 64

char greetings[]= "▀▄   ▄▀  ▀▄   ▄▀  ▀▄   ▄▀  ▀▄   ▄▀\n"
                   " █▄▀█ █▄▀█  █▄▀█ █▄▀█  █▄▀█ █▄▀█\n"
                   " █  █ █  █  █  █ █  █  █  █ █  █\n"
                   "▀   ▀▀   ▀▀   ▀▀   ▀▀   ▀▀   ▀▀   ▀\n"
                   "\n"
                   "  CPU TAMER - Tame your cores, save your battery\n";

char buffer[BUFFER_SIZE];
int current_cpu_state[THREADS], new_cpu_state[THREADS];

int get_current_cpu_state(void){
    for (int i=0;i<THREADS;i++){
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

/** defined commands:
2c2t, 2c3t, 2c4t, 3c3t, 3c4t, 3c5t, 3c6t
q, exit--> quit
smt --> show smt status: on/off
smt-on, smt-off
min --> 1c1t/1c2t
max --> enable all cores and threads
**/
int parse_input(){
    for (int i=0;i<BUFFER_SIZE;i++){

    }
}

void print_greetings(){
    printf("%s",greetings);
}

int cpuset_safety_checker(){//ensure not all cpus are disabled
    int sum=0;
    for(int i=0;i<THREADS;i++){
        sum+=new_cpu_state[i];
    }
    if(sum>0) return 0;//normal
    return 1;//wrong!!! Your CPU will be completely down!!
}

int main(){
  setenv("PATH", "/bin:/usr/bin:/usr/local/bin", 1);
  char user_input[256];
  //我们的代码
  while (1) {
    //获取当前工作目录
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));
    printf("skublash%s>$ ",cwd);//打印当前工作目录          ！假设：正常情况下cwd不可能返回NULL
    // 2. 读取用户输入
    // fgets 会从“标准输入”（stdin，通常是键盘）读取一行文本，并将其存入 user_input 数组。
    // sizeof(user_input) 确保了读取不会超过数组的容量，防止溢出，这是安全的关键。
    if (fgets(user_input,sizeof(user_input),stdin)!= NULL){
    //3. 回显用户输入
      printf("You entered: %s%s",user_input,"processing command...\n");
      // --- 2. 解析输入：去除末尾的换行符 '\n' ---
            // user_input 现在是 "ls\n\0"，我们需要把它变成 "ls\0\0"
            // 思路：找到换行符，并用字符串结束符 '\0' 覆盖它。
      user_input[strcspn(user_input,"\n")]='\0';
      
      // --- 3. 准备参数列表 for execvp ---
            // execvp 需要一个参数数组，例如执行 "ls -l" 需要数组： {"ls", "-l", NULL}
            // 我们现在先简单处理：把整个输入作为一个命令，参数列表就是 {user_input, NULL}
      char *args[64];// c语言没有动态的集合 不能声明空集合 更不能往集合里面append或者add元素
      int args_count=0;
      //第一次分割
      char *token = strtok(user_input," \t");
      //循环获取所有令牌
      while(token!=NULL && args_count<63){
        args[args_count]=token;
        args_count++;
        token=strtok(NULL," \t");
      } 
      args[args_count]=NULL;
      if(args_count==0){continue;}
      //检查是否是内部命令 例如cd,pwd 
      char *bcmdList[]={"cd","exit",NULL};
      char *matched_cmd=NULL;

      for (int i=0;bcmdList[i]!=NULL;i++)//内部命令匹配
      {
        if(strcmp(args[0],bcmdList[i]) == 0){
          matched_cmd=bcmdList[i];//存储匹配的内部命令指针
          break;
        }
      }

      if(matched_cmd!=NULL){
        printf("DEBUG: Built-in Command Matched!!! \nTrying to execute: %s\n", args[0]);
        if (strcmp(matched_cmd,"cd")==0){
          chdir(args[1]);
        }
        if(strcmp(matched_cmd,"exit")==0){
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
        // if (strcmp(args[0],"pwd")==0){
        //   printf("%s",cwd);
        // }


                // 使用 execvp 来执行命令。如果执行成功，这行代码以下的代码都不会运行。
        printf("DEBUG: Trying to execute: %s\n", args[0]);
        printf("DEBUG: PATH=%s\n", getenv("PATH"));
        execvp(args[0],args);
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
    }
  }
  return 0;
}
  