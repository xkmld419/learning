ps -ef|grep  abmmgr_pay|grep -v grep |awk  '{print $2}'|xargs  kill
rm /ABM/app/deploy/log/*abmmgr_pay*
make clean
make all
