#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_HISTORY 100

char* history[MAX_COMMAND_LENGTH];
int history_pids[MAX_COMMAND_LENGTH];
int history_count = 0;

void split_lines(char* line, char** dest);
void exec_cd(char* path);
void exec_history();
int exec_command(char** line);

int main(int argc, char* argv[]) {
    if (argc > 1) {
        char* path = getenv("PATH");
        char new_path[strlen(path) + strlen(argv[1]) + 2];
        snprintf(new_path, sizeof(new_path), "%s:%s", argv[1], path);
        setenv("PATH", new_path, 1);
    } else {
        exit(1);
    }

    while (1) {
        printf("$ ");
        fflush(stdout);
        char command[MAX_COMMAND_LENGTH] = {0};
        scanf("%[^\n]", command);
        getchar();
        char* line[MAX_COMMAND_LENGTH] = {0};
        history[history_count] = strdup(command);
        split_lines(command, line);
        if (line[0] == NULL) {
            continue;
        }
       
        history_count++;
        
        if (strcmp(line[0], "exit") == 0) {
            break;
        } else if (strcmp(line[0], "cd") == 0) {
            exec_cd(line[1]);
            continue;
        } else if (strcmp(line[0], "history") == 0) {
            exec_history(history_count, history_pids, history);
            continue;
        } else {
            int result = exec_command(line);
            if (result == -1) {
              history_count--;
                break;
            }
            if (result > 0) {
                history_pids[history_count] = result;
            }
        }
    }
}


void split_lines(char* line, char** dest) {
    char* token = strtok(line, " ");
    int i = 0;
    while (token != NULL) {
        dest[i++] = token;
        token = strtok(NULL, " ");
    }
    dest[i] = NULL;
}

void exec_cd(char* path) {
    if (chdir(path) != 0) {
        perror("chdir failed");
    }
}

void exec_history() {
    int i;
    int count = history_count > MAX_HISTORY ? MAX_HISTORY : history_count;

    history_pids[history_count] = getpid();
    history_count++;

    for (i = 0; i < count; i++) {
        printf("%d %s\n", history_pids[i], history[i]);
    }
}

int exec_command(char** line) {
    int pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return -1;
    }
    if (pid == 0) {
        int result = execvp(line[0], line);
        if (result == -1) {
            perror("execvp failed");
            return -1;
        }
    } else {
        wait(NULL);
        return pid;
    }
}
