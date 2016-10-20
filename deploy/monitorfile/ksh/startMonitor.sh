. ${HOME}/.profile
today=`date +%Y%m%d`


nohup /ABM/app/deploy/bin/monitorFile >/ABM/app/deploy/monitorfile/log/monitorFile_${today}.log  2>&1  &
