/*VER: 1*/ 
#    (c) 2003 BEA Systems, Inc. All Rights Reserved.
#ident    "@(#) samples/atmi/simpapp/ubbtibs_bill    $Revision: 1.5 $"
#Skeleton UBBCONFIG file for the TUXEDO Simple Application.
#Replace the <bracketed> items with the appropriate values.

*RESOURCES

IPCKEY          124315
MASTER          tibs_wf
MAXACCESSERS    500
MAXSERVERS      50
MAXSERVICES     200
MODEL           SHM
LDBAL           N

*MACHINES
DEFAULT:

<"hostname">  LMID     = tibs_wf
            APPDIR     = <"/bill/TIBS_HOME/bin">
            TUXCONFIG  = <"/bill/TIBS_HOME/tuxsvc/TUXCONFIG">
            TUXDIR     = <"/home/tuxedo/tuxedo8.1">
            ULOGPFX    = <"/bill/TIBS_HOME/tuxsvc/ULOG">
            MAXWSCLIENTS = 50


*GROUPS
GROUP1 
    LMID=tibs_wf    GRPNO=110

GROUP2
    LMID=tibs_wf    GRPNO=120

GRPWSL
    LMID=tibs_wf    GRPNO=190  OPENINFO=NONE 

*SERVERS
DEFAULT:
    CLOPT="-A"    RESTART=Y    GRACE=86400
        
server_monitor  SRVGRP=GROUP1 SRVID=11 MAXGEN=2
server_client   SRVGRP=GROUP2 SRVID=12 MAXGEN=2
WSL             SRVGRP=GRPWSL SRVID=70 MAXGEN=2 RESTART=Y CLOPT = <"-A -t -- -n //132.228.91.68:3190 -m 5 -M 20 -x 3 -T 5 -I 20">

*SERVICES

CHANGE_STATUS
MONITOR_LOGIN
MONITOR_LOGOUT
RECV_APPINFO
REFRESH_RUNPLAN
SEND_COMMAND

ACK_COMMAND
CLIENT_LOGIN
CLIENT_LOGOUT
GET_COMMAND
SEND_REMOTEMSG
