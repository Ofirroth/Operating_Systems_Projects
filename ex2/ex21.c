#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/fcntl.h>
#define IDENTICAL 1
#define DIFFRENT 2
#define SIMILAR 3
#define ERROR -1

int isSpace(char c);
int isLetter(char c);
int checkOtherCases(char c1, char c2);

//this function checks if a character is a space character
int isSpace(char c){
    if ((c== '\n') || (c == ' ')){
        return 1;
    }
    return 0;
}
//this function checks if a character is a Letter character
int isLetter(char c){
    if (c <= 122 && c >= 65){
        return 1;
    }
    return 0;
}
//this function use the "isLetter" function to check if it legal cases with lower and upper case
int checkOtherCases(char c1, char c2){
    if (isLetter(c1) && isLetter(c2)){
        if ((c1-c2 == 32) || (c2-c1 == 32)){
            return 1;
        }
    }
    return 0;
}

int main (int argc, char *argv[1]){
    char c1,c2;
    int readfile1=1, readfile2=1;
    //open two files that sent as an argumants
    int fd1= open(argv[1], O_RDONLY);
    int fd2= open(argv[2], O_RDONLY);
    //if we have a space we need to read more characters until a letter
    int nextCharf1 = 1;
    int nextCharf2=1;

    int state = IDENTICAL;

    while (readfile1 !=0 && readfile2 !=0)
    {
        if(nextCharf1){
            readfile1 = read(fd1,&c1,1);
            if (readfile1 == ERROR) {
            close(fd1);
            close(fd2);
            printf("Error in: read");
            exit(ERROR);
        }
        }
        else{
            nextCharf1=1;
        }
        if(nextCharf2){
            readfile2 = read(fd2,&c2,1);
            if (readfile2 == ERROR) {
            close(fd1);
            close(fd2);
            printf("Error in: read");
            exit(ERROR);
            }
        }
        else{
            nextCharf2=1;
        }
        //cheack if we not finish the file's 
        if((readfile1==0) || (readfile2 ==0)){
            break;
        }
        //start compare between two character
        if (c1==c2)
        {
            continue;
        }
        //check if the characters is with diff of upper or lower case
        else if (checkOtherCases(c1,c2) == 1)
        {
            //if we get here the file are'nt identical so we in case of similar
            state=SIMILAR;
            continue;
        }
        //check if one of the characters is '/n' or ' '
        else if (isSpace(c1) || isSpace(c2))
        {
            //the case that the first character is ' ' or '/n'
            if(isSpace(c1)){
                //continue to read the character's until get to a letter
                while ((isSpace(c1)) && (readfile1 != 0))
                {
                    readfile1 = read(fd1,&c1,1);
                    if (readfile1 == ERROR) {
                        close(fd1);
                        close(fd2);
                        printf("Error in: read");
                        exit(ERROR);
                        }
                }
                //we read until we get to a letter
                nextCharf1=0;
                nextCharf2=0;
                state=SIMILAR;
            }
            //the same like file1 and c1
            if(isSpace(c2)){
                while ((isSpace(c2)) && (readfile2 != 0))
                {
                    readfile2 = read(fd2,&c2,1);
                    if (readfile2 == ERROR) {
                        close(fd1);
                        close(fd2);
                        printf("Error in: read");
                        exit(ERROR);
                        }
                }
                nextCharf1=0;
                nextCharf2=0;
                state=SIMILAR;
            }
            //cheack if we not finish the file's 
            if((readfile1==0) || (readfile2 ==0)){
                break;
            }
        }
        //we get to the else just if we get two diffrent character's
        else{
            state=DIFFRENT;
            close(fd1);
            close(fd2);
            return state;
        }
    }
    //we finish the files already so we just return the state
    if((readfile1 == 0) && (readfile2 == 0)){
        close(fd1);
        close(fd2);
        return state;
    }
    //if one is longer that another we cheack if we have just spaces or '/n' and we return that the files are similar
    else if (readfile1>0){
        //in case the file not finish and we still have characteres to read so we need  just spaces and '/n'
        while(readfile1 !=0){
            if (!isSpace(c1)){
                state = DIFFRENT;
                close(fd1);
                close(fd2);
                return state;
            }
            readfile1 = read(fd1,&c1,1);
            if (readfile1 == ERROR) {
                close(fd1);
                close(fd2);
                printf("Error in: read");
                exit(ERROR);
                }
        }
        state = SIMILAR;
        close(fd1);
        close(fd2);
        return state;
        }
    else{
        //in case the file not finish and we still have characteres to read so we need  just spaces and '/n'
        while(readfile2 !=0){
            if (!isSpace(c2)){
                state = DIFFRENT;
                close(fd1);
                close(fd2);
                return state; 
            }
            readfile2 = read(fd2,&c2,1);
            if (readfile2 == ERROR) {
                close(fd1);
                close(fd2);
                printf("Error in: read");
                exit(ERROR);
                }
        }
        state = SIMILAR;
        close(fd1);
        close(fd2);
        return state; 
    }
}
    