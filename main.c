#include "types.h"
#include "syscall.h"
#include "stdlib.h"

bool shell_running = true;

static void replace_chars(char* str, size_t len,  char c, char r) {
    while(len--) {
        if(*str == c) {
            *str = r;
        }
        str++;
    }
}

static int countc(const char* str, char c) {
    int count = 0;
    while(*str) {
        if(*str++ == c) {
            count++;
        }
    }
    return count;
}

bool run_internal_cmd(char* cmd, char* argv[]) {
    int argc = 0;
    while(argv[argc]) {
        argc++;
    }
    
    if(!strcmp(cmd, "boop")) {
        if(argc > 1) {
            if(!strcmp(argv[1], "me")) {
                printf("*boops you* :3\n");
            }
            else {
                printf("*boops %s*\n", argv[1]);
            }
        }
        else {
            printf(":flushed:\n");
        }
        return true;
    }

    if(!strcmp(cmd, "exit")) {
        printf("https://tenor.com/view/crying-emoji-dies-gif-21956120\n");
        shell_running = false;
        return true;
    }

    return false;
}

int main(int argc, char* argv[], char* envp[]) {
    printf("envp:\n");
    size_t cntr = 0;
    while (envp[cntr]) {
        printf("%s\n", envp[cntr]);
        cntr++;
    }
    printf("argv:\n");
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    printf("welcome to the shitshell:tm:\n");

    char input_buf[100];

    while(shell_running) {
        printf("# ");
        sys_read(1, input_buf, sizeof(input_buf));
        replace_chars(input_buf, sizeof(input_buf), '\n', '\0');

        /* create argv array, null terminated */
        memcpy(input_buf, input_buf, strlen(input_buf));
        int arg_count = countc(input_buf, ' ') + 1; // count spaces in string
        char *arg_arr[arg_count +1];
        arg_arr[arg_count] = 0; // null termination
        char *strptr = input_buf;
        for(int i = 0; i < arg_count; i++) {
            arg_arr[i] = strptr;
            while(*strptr && *strptr != ' ') {
                *strptr++;
            }
            *strptr++;
        }
        replace_chars(input_buf, sizeof(input_buf), ' ', '\0');

        if(run_internal_cmd(arg_arr[0], arg_arr)) {
            continue;
        }
        
        pid_t forked = sys_fork();
        if(!forked) {
            sys_execve(arg_arr[0], arg_arr, 0);
            printf("execve failed!\n");
            return 1; // execve failed
        }
        for(uint32_t i = 0; i < 0xFFFFFF; i++) {}
        sys_waitpid(forked, 0, 0);

        memset(input_buf, 0, 100); // clear buffer
    }

    return 0;
}