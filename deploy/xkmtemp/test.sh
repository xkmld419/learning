#!/bin/sh
varT=`date "+%d"`
#echo $varT
ftp -i -n "132.129.24.25" <<FTPENV
user "idep" "idepctp"
lcd /ABM/app/deploy/xkmtemp/
bin
prompt off
cd /idep/IDEP/data/0/40/201108/$varT/0010/4009/40090001/
mget Dialmdn*
bye
FTPENV
