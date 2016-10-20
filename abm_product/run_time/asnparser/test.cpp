#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ASNDecode.h"

void getCur( )
{
        struct timeval tv;
        struct timezone tz;
        struct tm * p;

        int i = gettimeofday (&tv , &tz);
        printf("%d\n", (tv.tv_usec));
}

int main(int argc, char* argv[])
{
//    return ASNDBConf::getInstance()->checkField(NULL);
//    if (1 >= argc) return -1;
    ASNFileParser* oparser = ASNFileParser::getInstance(2);

    for (int i = 0; i < 1; i++) {
    if( oparser->decode(argv[1])) {
        printf("decode error");
        return 0;
    }
    printf("cur is %d ", i);
    }

    int fd = open("./yy.log", O_WRONLY);
    return oparser->getRecord()->fetchNode();
}
