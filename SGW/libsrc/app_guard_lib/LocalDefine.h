#ifndef __LOCAL_DEFINE__

// 此文件本地维护,无需提交配置库. 修改后需要重新编译生效

///////////////////////////////////////////////////////////////////////////////////////////
// 入库部分宏定义

    #define EVENT_AGGR_INDEX_NUM     1            //事件累积索引的个数(SERV_ID)
    #define ACCTITEM_AGGR_INDEX_NUM  1            //帐目累积索引的个数(SERV_ID)
    #define INDEX__SERV_ID           0                //第一索引SERV_ID
    #define ADD_AGGR_MAX_TABLE_NUM  16            //一条计费流程最大可能生成的增量累积表的个数
    
    #define EVENT_AGGR_DATA_COUNT        20000000        //事件累积数据段记录数
    #define EVENT_ADD_AGGR_DATA_COUNT    800000            //事件增量累积数据段记录数
    #define ACCTITEM_AGGR_DATA_COUNT     50000000        //帐目累积数据段记录数
    #define ACCTITEM_ADD_AGGR_DATA_COUNT 400000        //帐目增量累积数据段记录数
    #define EVENT_AGGR_INDEX_COUNT       3000000            //事件累积索引段记录数
    #define EVENT_ADD_AGGR_INDEX_COUNT   400000            //事件增量累积索引段记录数
    #define ACCTITEM_AGGR_INDEX_COUNT    4000000            //帐目累积索引段记录数
    #define ACCTITEM_ADD_AGGR_INDEX_COUNT    400000        //帐目增量累积索引段记录数
    #define MAX_EVENT_AGGR_TYPE_NUM_OfPerEvent 5        //一件事件最多可能存在的事件累积类型数量
    
    #define EVENT_AGGR_ID_INCREATE_SIZE  10000        //事件累积标识序列每次increate的大小
                                                     //-- 必须与数据库中EVENT_AGGR_ID_SEQ的increate值一致
    #define ACCT_ITEM_ID_INCREATE_SIZE   10000        //帐目标识序列每次increate的大小
                                                     //-- 必须与数据库中ACCT_ITEM_ID_SEQ的increate值一致
                                
    #define MAX_END_FILE            100000            //一次事务操作最多结束文件数量
    #define MAX_ACCT_ITEM_PER_EVENT        200            //一条事件记录最多可能产生的帐目记录
    #define TransactionCommit_EventNum    7000        //缓冲提交大小
    #define EVENT_MAX_EXTEND_ATTR_NUM    10            //事件最大的扩展属性个数
    #define MAX_STDEVENT_TABLE_NUM        5            //一次提交中最多可能产生的标准事件表的数量
    #define MAX_EXTEVENT_TABLE_NUM        5            //一次提交中最多可能产生的扩展事件表的数量
    #define MAX_REDOEVENT_TABLE_NUM        5            //一次提交中最多可能产生的回收事件表的数量
    #define MAX_NOOWNEREVENT_TABLE_NUM    5            //一次提交中最多可能产生的无主事件表的数量

    #define MAX_OTHEREVENT_TABLE_NUM    1        //一次提交中最多可能产生的other事件表的数量
    #define MAX_ROLLBACK_TABLE_NUM        1        //一次提交中最多可能产生的rollback事件表的数量
///////////////////////////////////////////////////////////////////////////////////////////


///////////////////
// 监控守护模块宏定义
#define MAX_APP_NUM 2048   //定义系统

///////////////////

#endif




