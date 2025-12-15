#include <linux_x86/syscall.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    if (!strcmp(argv[0], "help")) {
        printf(
            "shell builtins:\n"
            "help\n"
            "boop [name]\n"
            "sysinfo -- print output from sysinfo syscall\n"
            "exit\n"
            "clear\n"
            "fibonacci\n"
            "echo\n"
        );
        return true;
    }

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
                    if (i == (argc - 2)) {
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

    if (!strcmp(argv[0], "sysinfo")) {
        struct sysinfo info;
        if (sys_sysinfo(&info) != 0) {
            printf("sysinfo skill issue\n");
            return true;
        }
        printf("uptime(seconds): %u\ntotal RAM: %uMiB\nfree RAM: %uMiB\nprocesses: %u\n",
               (size_t)info.uptime,
               ((size_t)info.totalram * info.mem_unit) / 1048576,
               ((size_t)info.freeram * info.mem_unit) / 1048576,
               (size_t)info.procs);
        return true;
    }

    if (!strcmp(argv[0], "exit")) {
        printf("https://tenor.com/view/crying-emoji-dies-gif-21956120\n");
        exit(0);
        return true;
    }

    if (!strcmp(argv[0], "clear")) {
        printf("\e[1;1H\e[2J");
        return true;
    }

    if (!strcmp(argv[0], "fibonacci")) {
        unsigned long n_ = 12; // because shitcstd sscanf only supports long!
        if (argc > 1) {
            sscanf(argv[1], "%lu", &n_);
        }

        unsigned int n = n_;
        unsigned int number = 1;
        if (n != 0) {
            unsigned int number_prev = 0;
            for (unsigned int i = 1; i < n; i++) {
                unsigned int prev_tmp = number;
                number = number_prev + number;
                number_prev = prev_tmp;
            }
        } else {
            number = 0;
        }

        printf("F%u = %u\n", n, number);
        return true;
    }

    if (!strcmp(argv[0], "echo")) {
        for (int i = 1; i < argc; i++) {
            printf("%s ", argv[i]);
        }
        printf("\n");
        return true;
    }

    return false;
}

int main(int argc, char *argv[], char *envp[]) {
    printf("welcome to the shitshell:tm:\n");

    char input_buf[100];

    struct sysinfo info;
    sys_sysinfo(&info);
    printf("uptime(seconds): %u\ntotal RAM: %uMiB\nfree RAM: %uMiB\nprocesses: %u\n",
           (size_t)info.uptime,
           ((size_t)info.totalram * info.mem_unit) / 1048576,
           ((size_t)info.freeram * info.mem_unit) / 1048576,
           (size_t)info.procs);

    struct utsname uname_buf;
    sys_uname(&uname_buf);
    printf("running on: %s %s %s %s %s\n", uname_buf.sysname, uname_buf.nodename, uname_buf.release, uname_buf.version, uname_buf.machine);
    while (true) {
        char cwd[100];
        sys_getcwd(cwd, 100);
        printf("%s@%s:%s# ", "unknown", uname_buf.nodename, cwd); // we do not figure out the current username because that would require reading /etc/passwd which vix doesn't support _yet_

        sys_read(0, input_buf, sizeof(input_buf));
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

        if (strlen(arg_arr[0]) == 0) {
            goto cleanup_buf;
        }

        if (run_internal_cmd(arg_count, arg_arr)) {
            goto cleanup_buf;
        }

        pid_t forked = sys_fork();
        if (!forked) {
            sys_execve(arg_arr[0], arg_arr, 0);
            printf("execve failed!\n");
            return 1; // execve failed
        }
        sys_waitpid(forked, 0, 0);

        cleanup_buf:
            memset(input_buf, 0, 100); // clear buffer
    }

    return 0;
}
