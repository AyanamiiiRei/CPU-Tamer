#ifndef SYSTEM_EXECUTION_H
#define SYSTEM_EXECUTION_H

// 变量声明
// extern char *commands[];
// extern int commands_count;
// extern char *matched_command;

// 函数原型声明
// int get_commands(void);
// int match_first_cmd(void);
// int parse_input(void);
int execute_shell_command(const char *cmd);
int toggle_cpu_state(int list[]);
#endif