#include <iostream>
#include <time.h>

using namespace std;

//将时间转换为秒数
//返回值加上1900~1970的秒数+8小时时差
inline unsigned int date2long( long  x){
    int b ;
    int y;
    struct tm tmtime;  
    b =x/100000000;
    b= b/100;
    tmtime.tm_year=b-1900;
    b= ((x/1000)/100000)%100;
    tmtime.tm_mon=b-1;
    b= ((x/1000)/1000)%100;
    tmtime.tm_mday=b;
    b= ((x/1000)/10)%100;
    tmtime.tm_hour=b;
    b= (x/100)%100;
    tmtime.tm_min=b;
    b= x%100;
    tmtime.tm_sec=b;
    y=mktime(&tmtime);
    cout<<endl;
    return y+2209017599; 
}
//将秒数转换为字符串类型时间
//目前入参为从1900开始的秒数，要先减去从1900~1970的秒数再减去8小时时差再计算
inline int long2date(unsigned int U32Var,char *str)
{
   U32Var -=2209017599;
   time_t t;
   time_t nowtime;
   long x;    
   struct tm *ptm;
   nowtime = (time_t)U32Var;
   ptm = localtime(&nowtime);
   
   sprintf(str, "%04d%02d%02d%02d%02d%02d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec); 
   return 0;
};

