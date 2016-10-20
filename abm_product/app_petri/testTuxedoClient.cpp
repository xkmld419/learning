#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <atmi.h>         /* TUXEDO 头文件*/

int main(int argc, char *argv[])
{
       char *sendbuf, *rcvbuf;
       long sendlen, rcvlen;
       char svcname[16];
       int ret;

       if(argc != 3) {
              (void) fprintf(stderr, "arg:clientpetri servname sendbuf");
              exit(1);
       }

       /* 作为tuxedo客户端连入tuxedo System */

       if (tpinit((TPINIT *) NULL) == -1) {
              (void) fprintf(stderr, "Tpinit failed ");
              exit(1);
       }

       sendlen = strlen(argv[2]);
       /* 使用tpalloc申请string类型buffer作为请求和应答buffer */
       if((sendbuf = (char *) tpalloc("STRING", NULL, sendlen+1)) == NULL) {
              (void) fprintf(stderr,"Error allocating send buffer ");
              tpterm();
              exit(1);
       }

       if((rcvbuf = (char *) tpalloc("STRING", NULL, 1024*2)) == NULL) {
              (void) fprintf(stderr,"Error allocating receive buffer ");
              tpfree(sendbuf);
              tpterm();
              exit(1);
       }
       (void) strcpy(svcname, argv[1]);	       
       (void) strcpy(sendbuf, argv[2]);

       /* 同步调用服务 */
       ret = tpcall(svcname, (char *)sendbuf, 0, (char **)&rcvbuf, &rcvlen, (long)0);
       if(ret == -1) {
              (void) fprintf(stderr, "Can't send request to service ");
              (void) fprintf(stderr, "Tperrno = %d ", tperrno);
              tpfree(sendbuf);
              tpfree(rcvbuf);
              tpterm();
              exit(1);
       }

       (void) fprintf(stdout, "Returned string is: %s ", rcvbuf);
       /* 使用tpfree释放申请的请求和应答buffer空间 */
       tpfree(sendbuf);
       tpfree(rcvbuf);
       /* 使用tpterm离开tuxedo System */
       tpterm();
       return(0);
}

