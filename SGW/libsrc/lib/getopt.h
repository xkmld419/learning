/* 解决Aix上的编译问题(找不到getopt.h 头文件) */
#ifdef DEF_LINUX
#include </usr/include/getopt.h>
#endif
 



