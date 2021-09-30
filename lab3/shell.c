#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char gpath[128];  // hold token strings
char* arg[128];   // token string pointers
int n;            // number of token strings

char dpath[128];  // hold dir strings in PATH
char* dir[64];    // dir string pointers
int ndir;         // number of dirs

char* sequence[128];            // user ";" to split command
char* pipe_commands[128][128];  // user "|" to split command
char g_env_path[2048];          // hold evn path

int handle_redirect_in(char* argv[]) {
    char* infile = NULL;
    for (int i = 0; argv[i]; i++) {
        if (strcmp(argv[i], "<") == 0) {
            infile = argv[i + 1];
            argv[i] = NULL;
            break;
        }
    }
    if (infile == NULL)
        return 0;
    int infd = open(infile, O_RDONLY);
    dup2(infd, STDIN_FILENO);
    return close(infd);
}

int handle_redirect_out(char* argv[]) {
    char* outfile = NULL;
    int redirect_append = 0;
    for (int i = 0; argv[i]; i++) {
        if (strcmp(argv[i], ">") == 0) {
            outfile = argv[i + 1];
            argv[i] = NULL;
            break;
        }
        if (strcmp(argv[i], ">>") == 0) {
            outfile = argv[i + 1];
            argv[i] = NULL;
            redirect_append = 1;
            break;
        }
    }
    if (outfile == NULL)
        return 0;
#ifdef __MY_DEBUG
        // printf("outfile %s \n", outfile);
#endif
    int outfd = open(outfile, O_CREAT | O_RDWR, 0666);
    if (redirect_append == 0) {
        // printf("redirect_append %d \n", redirect_append);
        ftruncate(outfd, 0);
        lseek(outfd, 0, SEEK_SET);
    } else {
        lseek(outfd, 0, SEEK_END);
    }
    dup2(outfd, STDOUT_FILENO);
    return close(outfd);
}

void handle_redirect(char* argv[], int in_redirect, int out_redirect) {
#ifdef __MY_DEBUG
    for (size_t i = 0; argv[i]; i++) {
        // printf("arg[%d][ = %s \n", i, argv[i]);
    }
#endif
    if (out_redirect == 1) {
        if (handle_redirect_out(argv) == -1) {
            fprintf(stderr, "output redirect error");
            exit(1);
        }
    }
    if (in_redirect == 1) {
        if (handle_redirect_in(argv) == -1) {
            fprintf(stderr, "input redirect error");
            exit(1);
        }
    }
    execvp(argv[0], argv);
    exit(1);
}

void execute_command(char* argv[]) {
    int command_number = 0;
    int current = 0;
    memset(pipe_commands, 0, sizeof(pipe_commands));
    for (int i = 0; i < n; i++) {
        if (strcmp(argv[i], "|") == 0) {
            current = 0;
            command_number++;
            continue;
        }
        pipe_commands[command_number][current] = argv[i];
        current++;
    }
    command_number++;
#ifdef __MY_DEBUG
    // printf("%d......\n", count);
    for (size_t i = 0; i < count; i++) {
        for (size_t j = 0; pipe_commands[i][j]; j++) {
            // printf("arg[%d][%d] = %s \n", i, j, pipe_commands[i][j]);
        }
    }
#endif
    int child = 0;
    int fds[2] = {0};
    int i = 0;
    for (i = 0; i < command_number - 1; i++) {
        pipe(fds);
        child = fork();
        if (child) {
            dup2(fds[1], STDOUT_FILENO);
            close(fds[0]);
            close(fds[1]);
            handle_redirect(pipe_commands[i], i == 0, 0);
            exit(1);
        }
        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);
        close(fds[1]);
    }
    handle_redirect(pipe_commands[i], i == 0, 1);
    exit(1);
}

int tokenize_arg(char* command_string)  // YOU have done this in LAB2
{                                       // YOU better know how to apply it from now on
    char* s;
    strcpy(gpath, command_string);  // copy into global gpath[]
    s = strtok(gpath, " ");
    n = 0;
    while (s) {
        arg[n++] = s;  // token string pointers
        s = strtok(0, " ");
    }
    arg[n] = 0;  // arg[n] = NULL pointer
}

int tokenize_path(char* pathname) {
    char* s;
    strcpy(g_env_path, pathname);  // copy into global g_env_path[]
    s = strtok(g_env_path, ":");
    ndir = 0;
    while (s) {
        dir[ndir++] = s;  // token string pointers
        s = strtok(0, ":");
    }
    dir[ndir] = 0;  // dir[n] = NULL pointer
}

int main(int argc, char* argv[], char* env[]) {
    int i;
    int pid, status;
    char* cmd;
    char line[28];
    char* path_env_value = getenv("PATH");
    if (path_env_value == NULL) {
        // printf(" path env not found\n");
        return -1;
    }
    tokenize_path(path_env_value);
    // show dirs
    // for (i = 0; i < ndir; i++)
    // printf("dir[%d] = %s\n", i, dir[i]);

    while (1) {
        // printf("sh %d running\n", getpid());
        printf("enter a command line : ");
        fgets(line, 128, stdin);
        line[strlen(line) - 1] = 0;
        if (line[0] == 0)
            continue;

        const char* delims = ";";
        char* command;
        command = strtok(line, delims);
        int command_number = 0;
        while (command) {
            sequence[command_number] = command;
            command_number++;
            command = strtok(NULL, delims);
        }

        for (size_t current_index = 0; current_index < command_number; current_index++) {
            command = sequence[current_index];
            tokenize_arg(command);
            for (i = 0; i < n; i++) {  // show token strings
                // printf("arg[%d] = %s\n", i, arg[i]);
            }
            // getchar();
            cmd = arg[0];  // line = arg0 arg1 arg2 ...
            if (strcmp(cmd, "cd") == 0) {
                chdir(arg[1]);
                continue;
            }
            if (strcmp(cmd, "exit") == 0)
                exit(0);
            pid = fork();
            if (pid) {
                // printf("sh %d forked a child sh %d\n", getpid(), pid);
                // printf("sh %d wait for child sh %d to terminate\n", getpid(), pid);
                pid = wait(&status);
                // printf("ZOMBIE child=%d exitStatus=%x\n", pid, status);
                // printf("main sh %d repeat loop\n", getpid());
            } else {
                // printf("child sh ****************************** %d running\n", getpid());
                execute_command(arg);
                exit(1);
            }
        }
    }
}