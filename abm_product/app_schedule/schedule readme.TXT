sendfor qryserv_r 10000 //发送到82000队列,发送到解包，李建成
revcfor //接受的



[SCHEDULE] 

global_shift = 10  #进程个数调节因子
force_start_abort = 1 #强起abort进程的开关
water_check_interval = 10 #sleep(10)后做下一轮逻辑处理
process_start_time_out = 10  #进程启动超时阀值
online_auto_schedule = 0 #在online状态下是否启动高低水调度，一级开关
offline_auto_schedule = 0 #在offline状态下是否启动高低水调度，一级开关
maintenance_auto_schedule = 0 #在维保状态下是否启动高低水调度，一级开关
module_list = 1，4，6，5 #罗列了高低水的vp_id,用于信息点。
同时排列在前面的是源头进程，依次向下。这个是为了满足案例里面的如果上游进程最大进程数远远大于下游进程的进程数，需要告警。例如如果4配置的最大进程数比6的多2个以上，系统就会告警。

vp1.min_process = 1 #高低水的最低进程数
vp1.max_process = 1 #高低水的最大进程数
vp1.process_high_level = 80 #高低水的高水位，一般是消息队列的百分比
vp1.process_low_level = 10 #高低水的低水位
vp1.queue_size = 1000 #高低水的队列长度，即计算消息队列百分比时的分母
vp1.shake_time = 2 #防抖动时间，高水位或低水位持续到指定时候后才可启停进程，防止反复启停
vp1.hwm_switch = 0 #高低水二级开关，配0则不参与高低水调度
vp1.online_state = 1 #在网时状态，1是启动
vp1.offline_state = 0 #离网时状态，0是停止
vp1.maintenance_state = 2 #维保时状态，2是保持不变

vp2.min_process = 1 #高低水的最低进程数
vp2.max_process = 1 #高低水的最大进程数
vp2.process_high_level = 80 #高低水的高水位
vp2.process_low_level = 10 #高低水的低水位
vp2.queue_size = 100 #高低水的队列长度
vp2.shake_time = 2 #防抖动时间
vp2.hwm_switch = 0
vp2.online_state = 1 #在网时状态
vp2.offline_state = 0 #离网时状态
vp2.maintenance_state = 2 #维保时状态


...

逻辑说明：
wf_process新增字段: vp_id
vp是virtual processor的缩写，即虚处理器
同一流程同一组可并发的兄弟进程配置同为一vp_id,
相同vp_id的进程会被schedule视作一个进程组，
vp_id的值直接关系到核心参数的配置，
使用vp_id的首数字作为配置号，比如进程x的vp_id
是2001，那么该进程对应的就是使用vp2.xxxx的一系列配置
vp_id是3004，那么该进程对应的就是使用vp3_xxx的一系列配置


高低水的判断通过进程对应的消息队列的消息数和核心参数里面最大消息数的比例，来判断是需要停止进程，
还是启动进程。一个VP组的所有进程对应一个消息队列。测试高低水时需要使用模拟器对业务进程发送消息。业务进程的消息分发配置在etc/hss_app_cfg.ini文件里面。
大概是 hssserver 分发给 解包，解包把查询消息分发给hssquery,hssquery把消息分发给打包程序，打包程序在把消息分发给hssserver。

状态变迁的逻辑：
srmode发起系统状态发生变更，
0号进程会通知流程调度schedule，
这时流程调度开始做状态变迁，
首先停止数据源和特殊进程，
HSS中的数据源是hssserver进程，
然后等待所有下游进程任务处理完毕，
若此时有下游进程不在运行，
例如有个hssorder停了，但是消息队列里还有任务，
此时这个hssorder会被调起来，把队列打通，
然后检测第一批被停得进程有没有停下来，
如果没有就一直等待直到超时，如果停下来了，
则对所有下游进程发送停指令，然后检测他们有没有真实的停下来，
如果超时则系统变迁失败，在下次采样时间后不停的做变迁

进程的实际启停由GuardMain处理，
对schedule而言，发送启停指令后必须获知进程最终结果，
如果对某个进程进行了启动，那在稍后会检查其是否到达RUN状态，
如果对某个进程进行了停操作，那么稍后将检查其是否到达SLP,
若出现异常导致进程变KILL,ABORT也认为进程停止了，
也就是对于BOOT,INIT,Down这些中间态是需要继续检测的，
直到其到达目标态或超时失败。


总之一句话：要想稳定的停止系统，需要先处理数据源头，使数据源头处理干净，
然后系统中没有消息残留，才能停后续进程，如果有进程停不下来，则系统变更失败


特殊的：哪个进程是数据源进程是写死的 ，函数是int VirtualProcessor::IsDataSrc()
        用wf_application的app_id判的
        
        系统第一次启动不做变更，即认为到达目标态，比如现在核心参数是online,此时切换到offline
        则schedule会按照vpx中各虚处理器在offline下的描述做变更。
        在hss中 发起系统状态变更的是hssmode
        
        
高低水的逻辑：
比如现在系统是online，且online_auto_schedule = 1（一级开关打开）
vp1.hwm_switch = 1（二级开关打开），那么vp1涉及的进程会做高低水

通过判断消息队列的消息数和消息队列的比来判断水位，再通过水位的状态持续时间
来做新起或停止进程的操作


 

