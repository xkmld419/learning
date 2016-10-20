#!/bin/sh

/ABM/app/deploy/xkmtemp/test.sh

var9=`ls -lart /ABM/app/deploy/xkmtemp/|grep -v xkm|grep Dialmdn|tail -1|awk '{print $8}'`
echo $var9

echo '### abmmgr' >/ABM/app/deploy/xkmtemp/xkm.txt
echo '<TEST_CONF>' >>/ABM/app/deploy/xkmtemp/xkm.txt
echo '&|number            |area|' >>/ABM/app/deploy/xkmtemp/xkm.txt
awk -F"|" '{OFS="|"}{print "*|"$3,$2"|"}' /ABM/app/deploy/xkmtemp/$var9 >>/ABM/app/deploy/xkmtemp/xkm.txt
echo '<TEST_CONF>' >>/ABM/app/deploy/xkmtemp/xkm.txt
cp /ABM/app/deploy/xkmtemp/xkm.txt /ABM/app/deploy/config/abm_test_number.ini
mv /ABM/app/deploy/xkmtemp/xkm.txt /ABM/app/deploy/xkmtemp/xkm$var9.txt
