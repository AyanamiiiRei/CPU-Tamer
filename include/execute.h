// execute.h
#ifndef EXECUTE_H
#define EXECUTE_H

#define LOGICAL_CPU_COUNT 8
#define PHYSICAL_CPU_COUNT 4

// 变量声明
extern char greetings[];
extern int current_cpu_state[]; // 注意：这里可以不写大小，或者写 [LOGICAL_CPU_COUNT]
extern int new_cpu_state[];

// 函数原型声明
void print_greetings(void);
int get_current_cpu_state(void);
int handle_first_command(char* first_command, char **cmds);
// 内部辅助函数通常不需要放在 .h 里暴露出去，除非其他文件要用
// 比如 execute_shell_command, set 等如果是内部用的，可以不放这里
// 但为了 main.c 能调用 handle_first_command，必须放这里

#endif
