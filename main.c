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
            sys_write(1, "*", 1);
            if(!strcmp(argv[1], "me")) {
                sys_write(1, "boops you", 9);
            }
            else {
                sys_write(1, argv[1], strlen(argv[1]));
                sys_write(1, " boops back", 11);
            }
            sys_write(1, "*", 1);
            if(!strcmp(argv[1], "me")) {
                sys_write(1, " :3", 3);
            }
        }
        else {
            sys_write(1, ":flushed:", 10);
        }
        sys_write(1, "\n", 1);
        return true;
    }

    if(!strcmp(cmd, "exit")) {
        sys_write(1, "https://tenor.com/view/crying-emoji-dies-gif-21956120\n", 54);
        shell_running = false;
        return true;
    }

    return false;
}

int main(int argc, char* argv[], char* envp[]) {
    sys_write(1, "envp:\n", 6);
    size_t cntr = 0;
    while (envp[cntr]) {
        sys_write(1, envp[cntr], strlen(envp[cntr]));
        sys_write(1, "\n", 1);
        cntr++;
    }
    sys_write(1, "argv:\n", 6);
    for (int i = 0; i < argc; i++) {
        sys_write(1, argv[i], strlen(argv[i]));
        sys_write(1, "\n", 1);
    }

    sys_write(1, "welcome to the shitshell:tm:\n", 29);

    char input_buf[100];

    while(shell_running) {
        sys_write(1, "# ", 2);
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
            sys_write(1, "execve failed!\n", 15);
            return 1; // execve failed
        }
        for(uint32_t i = 0; i < 0xFFFFFF; i++) {}
        sys_waitpid(forked, 0, 0);

        memset(input_buf, 0, 100); // clear buffer
    }

    return 0;
}