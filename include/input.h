// input.h
#ifndef INPUT_H
#define INPUT_H

// 防止重复包含的保护宏 (Include Guards)
// 如果 INPUT_H 没定义过，就定义它并编译下面的内容
// 如果已经定义过，就跳过，避免重复声明报错

extern char buffer[]; // 告诉其他文件：buffer 在别处定义了

int get_input(void);  // 函数原型声明

#endif