#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "input.h"
#include "parser.h"
#include "execute.h"

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

int toggle_cpu_state(const char *path, int cpu_id, int state) {
    FILE *fp = fopen(path, "w");
    if (!fp) {
        // 使用 fprintf  stderr 而不是 perror，避免干扰主程序输出流
        fprintf(stderr, "Error: Failed to open %s for CPU %d: %s\n", 
                path, cpu_id, strerror(errno));
        return -1;
    }

    // 写入状态 (0 或 1)
    if (fprintf(fp, "%d", state) < 0) {
        fprintf(stderr, "Error: Failed to write to %s for CPU %d: %s\n", 
                path, cpu_id, strerror(errno));
        fclose(fp);
        return -1;
    }

    // 显式关闭文件，确保数据刷入磁盘
    if (fclose(fp) != 0) {
        fprintf(stderr, "Error: Failed to close %s for CPU %d: %s\n", 
                path, cpu_id, strerror(errno));
        return -1;
    }

    return 0;
}


