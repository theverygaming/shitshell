#pragma once
#include "types.h"

extern uint32_t syscall(uint32_t syscall_num, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);

typedef int pid_t;

void sys_exit(int error_code);
void sys_write(uint32_t fd, const char* buf, size_t count);
uint32_t sys_read(uint32_t fd, char* buf, size_t count);
void sys_waitpid(pid_t pid, int* stat_addr, int options);
pid_t sys_fork();
void sys_execve(const char* filename, char** argv, const char *const *envp);

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
    char domainname[65]; 
};

int sys_uname(struct utsname *buf);
char *sys_getcwd(char *buf, size_t size);
