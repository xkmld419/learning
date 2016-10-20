#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include "in.h"
using namespace std;
//extern unsigned int fx = 2;
//extern unsigned int fx;

int print(long i) {
    printf("unsigned int is %d\n", i);
    return 0;
}
int print(short i) {
    printf("short is %d\n", i);
    return 0;
}

int print(short i, int j = 10) {
    printf("short is %d\n", i);
    printf("j is %d\n", j);
    return 0;
}

int main()
{
    int xx = 1;
    print(xx);
    return 0;



    int fd1 = open("./yy.log", O_RDONLY);
    int fd2 = open("./yy.log", O_RDONLY);
    FILE* fd3 = fopen("./yy.log", "rb");
    printf("fd1 is %d ;", fd1);
    printf("fd2 is %d ;", fd2);
    printf("fd3 is %d ;", fd3);
    printf("\n");
    close(fd1);
    close(fd2);
    fclose(fd3);
    return 0;
    char x = '9';
    printf("x is %d \n", int(x) - 48);
    return 0;
    int child;

    char* args[] = {"/bin/echo", "Hello", "World!", NULL};

    if(!(child=fork())) {
        printf("pid %d : %d is myfather\n", getpid(), getppid());
        sleep(2);
        execve("/bin/echo", args, NULL);
        sleep(2);
        printf("pid %d : I am back, something is wrong!\n", getpid());
    }
    else {
        int myself = getpid();
        printf("pid %d : %d is son\n", myself, child);
//        wait4(child, NULL, 0, NULL);
        for (int i = 0; i < 10; i++) {
            printf("pid %d :%d\n", myself, i);
        }
        printf("pid %d :done\n", myself);
    }
    return 0;
}
