//ofir Rothchild 207434960

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// Adds the argument to the PATH environment variable
void AddPath (char* argv);

// Splits a command line into an array of strings
void splitlines (char* line, char** dest);

// Changes the current working directory to the specified path
void Cd (char* path);

// Prints the command history to the console
void History (int counter_history,int* pid_history,char **history_of_commands);

// Executes a command using execvp
int exec (char** line);


int main(int argc, char *argv[]){
    int i, counter = 2;
    //add the current folder
    // Adding all arguments to path environment variable
    for (i = 1; i < argc; i++) {
        AddPath(argv[i]);
    }
    //in order to save the commmands in array
    char *history_of_commands[101];
    int pid_history[101];
    int counter_history=0;
    while (1)
    {
        //print the prompt
        printf("$ ");
        fflush(stdout);
        char command[101] = {0};
        scanf("%[^\n]", command);
        getchar();
        char* line[100] = {0};
        //save the command in array of history command
        history_of_commands[counter_history] = strdup(command);

        splitlines(command, line);
        if(line[0]==NULL){
            continue;
        }
        pid_history[counter_history] = getpid();

        //check if the command is one of the built-in function

        // Exit the shell
        if (strcmp(line[0], "exit") == 0){
            break;
        }

        // Change directory
        if (strcmp(line[0], "cd") == 0){
            // Change the directory to the path specified in the second argument
            Cd(line[1]);
            counter_history++;
            continue;
        }

        // Print command history
        if (strcmp(line[0], "history") == 0) {
            History(counter_history, pid_history, history_of_commands);
            counter_history++;
            continue;
        }

        //the command is not one of the 3 built-in commands so we need to call execvp
        int result = exec(line);
        if (result == -1){
            break;
        }
        if (result > 0){
            pid_history[counter_history] = result;
        }

        counter_history++;
    }
}

// Adds the argument to the PATH environment variable
void AddPath(char* argv) {
    char path[1001];
    // Get the current value of the PATH environment variable
    strcpy(path, getenv("PATH"));
    // Append a colon to separate the new path from the existing ones
    strcat(path, ":");
    // Append the new path
    strcat(path, argv);
    // Set the new value of the PATH environment variable
    setenv("PATH", path, 1); 
}

// Splits a command line into an array of strings
void splitlines(char* line, char** dest){
    char *token = strtok(line, " ");
    int i = 0;
    while (token != NULL) {
        dest[i++] = token;
        token = strtok(NULL, " ");
    }
    // Add a null terminator to the end of the array
    dest[i] = NULL; 
}

// Changes the current working directory to the specified path
void Cd (char* path){
    int check = chdir(path);
    if (check != 0) {
        // Print an error message if the directory change
        perror("chdir failed"); 
    }
}
//prints the history of commands
void History(int counter_history,int* pid_history,char **history_of_commands){
    int i;
    for (i = 0; i <= counter_history; i++) {
        printf("%d %s\n", pid_history[i], history_of_commands[i]);
    }
}
//executes the command that the user entered
int exec (char** line){
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
