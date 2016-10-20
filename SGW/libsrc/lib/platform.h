#ifndef __PLATFORM_H_
#define __PLATFORM_H_

// HP
#if defined(__hpux) || defined(__HP_aCC) || defined(DEF_HP)

        #ifndef _POSIX_PTHREAD_SEMANTICS
        #define _POSIX_PTHREAD_SEMANTICS
        #endif

    #include <iostream.h>
    #include <fstream.h>
    #include <sstream>
    #include <string>
    #include <strings.h>
    #include <string.h>
typedef bool boolean_t;    

#endif

#if defined(_AIX)  || defined(DEF_AIX)

    #include <iostream>
    #include <fstream>
    #include <sstream>    
        using namespace std;     

#endif
#if defined(DEF_LINUX)

    #include <iostream>
    #include <fstream>
    #include <sstream>
    #include <signal.h>
        using namespace std;

#endif

        #include <stdio.h>
        #include <string.h>
        #include <stdlib.h>
        #include <ctype.h>
        
        // unix
        #include <unistd.h>
        #include <fcntl.h>
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <dirent.h>
        #include <fnmatch.h>
        #include<errno.h>
        
        // c++
        #include <map>
        #include <string>
        #include <vector>
        #include <exception>
        //#include <iostream>
        #include <utility>
        #include <set>

        // net
        #include<sys/socket.h>
        #include<arpa/inet.h>
        #include<netinet/in.h>
        #include<sys/param.h>

        // thread
        #include <pthread.h>

#endif



