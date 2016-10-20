#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
using namespace std;
int main()
{
system("ps -ef|grep xkmtest|grep -v grep|awk '{print $2}'|xargs kill -9");
return 0;
}


