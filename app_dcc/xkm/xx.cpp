#include <stdio.h> 
#include <time.h> 
#include <stdlib.h>

int TtoD(int a){
time_t nowtime;
int n; 
char *str; 
struct tm *ptm;

nowtime=(time_t)a;
ptm = localtime(&nowtime);

snprintf(str,14, "%04d%02d%02d%02d%02d%02d",
   ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
   ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
n = atoi(str);
 return n;
}


int main(void) 
{ 
   time_t t; 
   time_t nowtime;
   int U32Var=234567834;

   struct tm *ptm;
   
   t = time(NULL); 
   printf("Number of seconds since 1/1/1970 is: %ld\n", t); 
   /***
   nowtime = (time_t)U32Var;
   ptm = localtime(&nowtime);
   
   printf( "%04d%02d%02d%02d%02d%02d",
   ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
   ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
   ***/
   U32Var =TtoD(U32Var);
   
   getchar();
   return 0; 
} 

