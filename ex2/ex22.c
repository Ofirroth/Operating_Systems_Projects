#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#define ERROR -1
#define NO_C_FILE 0
#define COMPILATION_ERROR 1
#define TIMEOUT 2
#define WRONG 3
#define SIMILAR 4
#define EXCELLENT 5

char *findCFile(char *path)
{
    DIR *dip;
    struct dirent *dit;
    if ((dip = opendir(path)) == NULL)
    {
        printf("Eror in : opendir\n");
        return 0;
    }
    // open file resault
    while (((dit = readdir(dip)) != NULL))
    {
        char *fName = dit->d_name;
        // in order to not enter one more time
        if ((!strcmp(fName, ".")) || (!strcmp(fName, "..")))
        {
            continue;
        }
        int nLen = strlen(fName);
        // the file end with .c
        if ((fName[nLen - 1] == 'c') && (fName[nLen - 2] == '.'))
        {
            return fName;
        }
    }
    return NULL;
}

int readLines(char *str, int fd)
{
    char c;
    int output = read(fd, str, 450);
    int check = read(fd, &c, 1);
    if (check)
    {
        printf("the file to long\n");
        return ERROR;
    }
    return output;
}

int writeLines (char *name, int resaultFd,int score){
    int size = strlen(name);
    int writeRes=0;
    char arrName [151] = {0};
    //לממש strncpy 
    strcpy(arrName, name);
    arrName[size - 2] = '\0';
    arrName[size - 1] = '\0';
    arrName[size - 0] = '\0';
    char* results []={",0,NO_C_FILE\n", ",10,COMPILATION_ERROR\n", ",20,TIMEOUT\n", ",50,WRONG\n", ",75,SIMILAR\n", ",100,EXCELLENT\n"};
    strcat(arrName, results[score]);
    return writeRes = write(resaultFd, arrName, strlen(arrName));
}

int is_dir(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
    {
        // stat function failed, return false
        return 0;
    }
    return S_ISDIR(path_stat.st_mode);
}
// Splits a command line into an array of strings
void splitlines(char *str, char **dest)
{
    char *token = strtok(str, "\n");
    int i = 0;
    while (token != NULL)
    {
        dest[i++] = token;
        token = strtok(NULL, "\n");
    }
    // Add a null terminator to the end of the array
    dest[i] = NULL;
}


int execGcc(char* path,int error) {
    int status = 0;
    char *args[] = {"gcc", "-o","d.out",path, NULL};
    int pid = fork();
    if (pid == 0) {
        dup2(error,2);
        errno = 0;
        int result = execvp(args[0], args);
        if (result == -1) {
            exit(1);
        }

    } else if (pid > 0) {
        wait(&status);
        if ((WIFEXITED(status) )&& (WEXITSTATUS(status)==0)) {
            //printf("success\n");
            return 0;
        } else {
            //printf("error\n");
            return 1;
        }
    } else {
        printf("fork failed\n");
        return -1;
    }
}

// int execGcc(char *path, int fdError)
// {
//     char *args[] = {"gcc","-o","d.out" ,path, NULL};
//     int pid = fork();

//     if (pid < 0)
//     {
//         perror("fork failed");
//         return -1;
//     }

//     if (pid == 0)
//     {
//         dup2(fdError, 2);
//         int result = execvp(args[0], args);
//         if (result == -1)
//         {
//             perror("execvp failed");
//             return -1;
//         }
//     }
//     else
//     {
//         int status;
//         wait(&status);
//         if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
//         {
//             return 0;
//             // printf("Compilation successful\n");
//         }
//         else
//         {
//             return 1;
//             // printf("Compilation failed\n");
//         }
//         return pid;
//     }
// }

//executes the gcc command that the user entered
// int execOut (int fdInput, int fdOutput)
// {
//     int status=0;
//     char *args[] = {"./c.out", NULL};
//     int pid = fork();
//     if (pid == 0)
//     {
//         dup2(fdInput,0);
//         dup2(fdOutput,1);
   
//         int result = execvp(args[0], args);
//         if (result == -1)
//         {
//             perror("execvp failed");
//             exit(1);
//         }
//     }
//     else if (pid> 0)
//     {
//         printf("ori\n");
//         wait(&status);
//         if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
//         {
//             return 1;
//             // printf("Compilation successful\n");
//         }
//         else
//         {
//             return 0;
//             // printf("Compilation failed\n");
//         }
//     }
//     else
//     {
//         printf("fork failed");
//         return -1;
//     }
// }

int execOut (int fdInput , int fdOutput){
    int status = 0;
    char *args[] = {"./d.out", NULL};
    //end time end start time
    //double time=0;
    time_t tStart, tEnd;
    time(&tStart);
    //printf("%ld\n", (long)tStart);
    int pid = fork();
    if (pid == 0) {
        dup2(fdInput,0);
        dup2(fdOutput,1);
        int result = execvp(args[0], args);
        if (result == -1) {
            exit(1);
        }

    } else if (pid > 0) {
        wait(&status);
        time(&tEnd);
        double tRun = difftime(tEnd, tStart);
        //double fixedTime = ((double)tRun)*1000.0/CLOCKS_PER_SEC;
        printf("%f\n",tRun);
        if(tRun > 5){
            //timeOut
            return 2;
        }
        if ((WIFEXITED(status) )&& (WEXITSTATUS(status)==0)) {
            //printf("success\n");
            return 1;
        } else {
            //printf("error\n");
            return 0;
        }
    } else {
        printf("fork failed\n");
        return -1;
    }
}

int main(int argc, char *argv[])
{
    int state = 0;
    DIR *dip;
    struct dirent *dit;
    char str[450] = {0};
    char *lines[3] = {0};
    int fd = open(argv[1], O_RDONLY);
    state = readLines(str, fd);
    if (state == ERROR)
    {
        printf("Not a valid directory\n");
    }
    // check if error
    splitlines(str, lines);
    if ((dip = opendir(lines[0])) == NULL)
    {
        // printf("%s\n", lines[0]);
        printf("Eror in : opendir\n");
        return 0;
    }
    // open file resault
    int resultFd = open("results.csv", O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
    int errorFd = open("error.txt",  O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
    if (resultFd == -1)
    {
        printf("Eror in : open\n");
    }
    while (((dit = readdir(dip)) != NULL))
    {
        char path[301] = {0};
        char fileName[151] = {0};
        strcpy(fileName, dit->d_name);
        if ((!strcmp(fileName, ".")) || (!strcmp(fileName, "..")))
        {
            continue;
        }
        strcpy(path, lines[0]);
        strcat(path, "/");
        strcat(path, fileName);
        if ((is_dir(path)))
        {
            char *name = findCFile(path);
            if (name)
            {
                strcat(path, "/");
                strcat(path, name);
                // printf("%s\n", path);
                int gccResult = execGcc(path, errorFd);
                if(gccResult == -1){
                    printf("Eror in : execvp\n");
                }
                //CHANGE THE NUMBER
                else if (gccResult == 1)
                {
                //printf("compilation error\n");
                    int writeBytes = writeLines(fileName,resultFd,1);
                    if (writeBytes < strlen(fileName)){
                        printf("Eror in : write\n");
                    }
                    continue;
                }
                else
                {
                   //execute the c files and send the output of the files to new file call out.txt

                    strcat(fileName, ".txt");    
                    printf("%s\n", fileName);              
                    int fdOut= open(fileName,O_RDWR | O_CREAT, 0644);
                    int fdInput = open(lines[1], O_RDONLY);   
                    int fdCompare = open(lines[2], O_RDONLY);
                    int outResult = execOut(fdInput,fdOut);
                    close(fdOut);
                    close(fdInput);
                    close(fdCompare);
                    if (outResult == -1)
                    {
                        printf("Eror in : execvp\n");
                        break;
                    }
                    else if(outResult == 2){
                        int writeBytes = writeLines(fileName,resultFd,2);
                    if (writeBytes < strlen(fileName)){
                        printf("Eror in : write\n");
                    }
                    continue;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            else
            {
                writeLines(fileName,resultFd,0);
                continue;
            }
            
        }

    }
    // closedir(dip);
    // return 0;
}