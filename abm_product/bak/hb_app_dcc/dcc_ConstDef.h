/*VER:1*/



#ifndef __DCC_CONST_DEF_H__

#define __DCC_CONST_DEF_H__



#include <vector>

#include <map>

#include <string>

#include <stdlib.h>


const int FILE_FIELD_LENGTH = 1024;

const int FILE_NAME_LENGTH = 128;

const int FILE_PATHER_LENGTH = 30;

const int SESSION_ID_LENGTH = 120;



const int FILE_NONE_TYPE = 0;

const int FILE_HEAD_TYPE = 1;

const int FILE_BODY_TYPE = 2;

const int FILE_TAIL_TYPE = 3;



//DCC消息头中的hop字段的seq步长

const int DCC_SEQ_STEP = 500;

// 网元在网状态
typedef enum {
    ONLINE,
    OFFLINE
} Bsn_state;

/*
 *0-P0_Online_正常，激活CCR发送成功后 （在dccUnPack判断激活CCR是否成功）
 *1-P1_Online超负荷，消息来不及处理，队列阻塞，（dccTran每次发送CCA后判断发送CCA量与接收CCR量相差超过某个值）
 *2-P2_Online故障，消息无法解析 （在dccUnPack判断），基本不会出现
 *3-P3_Maintenance，正常退出 （在dccTran判断父进程退出，子进程跟着退出）
 *4-P4_Offline正常，正常退出 （在dccTran判断父进程退出，子进程跟着退出），与3重复
 *5-P5_Offline_故障，非正常退出 （在dccTran判断发送消息失败，可能由SR停掉导致）
 */
typedef enum {
    ONLINE_NORMAL,
    ONLINE_OVERLOAD,
    ONLINE_FAULT,
    MAINTENCE,
    OFFLINE_NORMAL,
    OFFLINE_FAULT
}Ne_state;


// 存储CCR的serviceType到共享内存
#define SHM_ITEM_NUM 10000      // serviceType共享内存元素个数
#define SHM_ITEM_LEN 50        // serviceType共享内存每个元素的长度

#endif

