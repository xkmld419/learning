#include "ShowMemData.h"

int main(int argc, char *argv[])
{
   
    ShmMgr showmem;
    showmem.g_argc=argc;
    showmem.g_argv=argv;
    showmem.show();

}
