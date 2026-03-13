// parser.h
#ifndef PARSER_H
#define PARSER_H

#define COMMAND_COUNT 64

// 变量声明
extern char *commands[];
extern int commands_count;
extern char *matched_command;

// 函数原型声明
int get_commands(void);
int match_first_cmd(void);
int parse_input(void);

#endif
