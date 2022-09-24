#include "stdlib.h"
#include "syscall.h"
#include "types.h"

static void replace_chars(char *str, size_t len, char c, char r) {
    while (len--) {
        if (*str == c) {
            *str = r;
        }
        str++;
    }
}

static int countc(const char *str, char c) {
    int count = 0;
    while (*str) {
        if (*str++ == c) {
            count++;
        }
    }
    return count;
}

bool run_internal_cmd(int argc, char *argv[]) {
    if (!strcmp(argv[0], "boop")) {
        if (argc > 1) {
            printf("*boops ");
            for (int i = 1; i < argc; i++) {
                if (!strcmp(argv[i], "me")) {
                    printf("you");
                } else {
                    printf("%s", argv[i]);
                }

                if (i != (argc - 1)) {
                    if(i == (argc - 2)) {
                        printf(" and ");
                    } else {
                        printf(", ");
                    }
                }
            }
            if (argc > 2) { // booped multiple
                printf("* :3 OwO\n");
            } else {
                printf("* :3\n");
            }
        } else {
            printf("usage: boop [name]\n");
        }
        return true;
    }

    if (!strcmp(argv[0], "exit")) {
        printf("https://tenor.com/view/crying-emoji-dies-gif-21956120\n");
        exit(0);
        return true;
    }

    return false;
}

int main(int argc, char *argv[], char *envp[]) {
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

    struct utsname uname_buf;
    sys_uname(&uname_buf);
    printf("running on: %s %s %s %s %s\n", uname_buf.sysname, uname_buf.nodename, uname_buf.release, uname_buf.version, uname_buf.machine);
    while (true) {
        char cwd[100];
        sys_getcwd(cwd, 100);
        printf("%s@%s:%s# ", "unknown", uname_buf.nodename, cwd); // we do not figure out the current username because that would require reading /etc/passwd which shitOS doesn't support _yet_

        sys_read(1, input_buf, sizeof(input_buf));
        replace_chars(input_buf, sizeof(input_buf), '\n', '\0');
        /* create argv array, null terminated */
        memcpy(input_buf, input_buf, strlen(input_buf));
        int arg_count = countc(input_buf, ' ') + 1; // count spaces in string

        if (input_buf[strlen(input_buf) - 1] == ' ') { // if string ends with space reduce arg_count by one to prevent passing an entirely empty argument
            arg_count--;
        }

        char *arg_arr[arg_count + 1];
        arg_arr[arg_count] = 0; // null termination
        char *strptr = input_buf;
        for (int i = 0; i < arg_count; i++) {
            arg_arr[i] = strptr;
            while (*strptr && *strptr != ' ') {
                strptr++;
            }
            strptr++;
        }
        replace_chars(input_buf, sizeof(input_buf), ' ', '\0');

        if (run_internal_cmd(arg_count, arg_arr)) {
            continue;
        }

        pid_t forked = sys_fork();
        if (!forked) {
            sys_execve(arg_arr[0], arg_arr, 0);
            printf("execve failed!\n");
            return 1; // execve failed
        }
        for (uint32_t i = 0; i < 0xFFFFFF; i++) {}
        sys_waitpid(forked, 0, 0);

        memset(input_buf, 0, 100); // clear buffer
    }

    return 0;
}
