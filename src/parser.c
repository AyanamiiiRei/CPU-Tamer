#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "input.h"
#include "parser.h"
#include "execute.h"

#define COMMAND_COUNT 64
#define LOGICAL_CPU_COUNT 8
#define PHYSICAL_CPU_COUNT 4

// extern char buffer[];
int commands_count=0;
char *commands[COMMAND_COUNT];
char *matched_command=NULL;
static char *defined_commands[]={"set","q","smt",NULL};

int get_commands(){
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
        return 1;
      }
      return 0;//parsed all commands and saved to commands[], return 0
}



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