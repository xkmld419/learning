/*VER: 1*/ 
#ifndef __TICKET_TLV_H__
#define __TICKET_TLV_H__

#include <string.h>
#include <stdlib.h>

//最大支持的TAG标识
#define MAX_TLV_FLAGS 1024

//一个tag位数据的最大长度(字节)
#define MAX_FIELD_LEN 200

#include "CommonMacro.h"
#include "Exception.h"

//## Tlv 话单存储格式(动态申请)
class TicketTlv {
  public:
    TicketTlv ()
    {
        memset (m_lAddress,  0, sizeof(unsigned long)*(MAX_TLV_FLAGS+1));
        memset (m_iFieldLen, 0, sizeof(int)*(MAX_TLV_FLAGS+1));
        memset (m_iTlvLen,   0, sizeof(int)*(MAX_TLV_FLAGS+1));
    }
    
    ~TicketTlv () 
    {
        for (int i=0; i<=MAX_TLV_FLAGS; i++) {
            if (m_lAddress[i])
                free ((void *)m_lAddress[i]);
        }
    }
        
    // 当前TicketTlv 所有tag位数据清空
    void reset ();

    // 根据tag值获取相应的字段值, Tlv数据有效长度, 字段长度
    // _iTag      : TAG标识
    // pValue     : 返回的字段值的地址
    // piTlvLen   : Tlv数据有效长度
    bool getTagValue (int _iTag, unsigned char *pValue, int &iTlvLen);

    // 置Tag位的值
    bool setTagValue (int _iTag, void *pValue, int _iTlvLen);
        
  private:
    // 申请一个Tag字段节点  return: 0--OK, -1--Malloc Err, 1--Tag位不合法
    int mallocField (int _iTag, int _iLen) ;
    
    // 校验,调整Tag位字段, 满足实际长度(_iTlvLen)的需要
    void ajustTagLen (int _iTag, int _iTlvLen);

    // 指定tag位数据清空
    void resetField (int _iTag);
    
  private:  
    //记录每个tag位对应的地址, tag_N 对应 m_Address[N]
    unsigned long m_lAddress[MAX_TLV_FLAGS+1];
    
    //记录每个tag位申请的内存大小(字节数)
    int           m_iFieldLen[MAX_TLV_FLAGS+1];
    
    //记录每个tag位当前数据实际有效长度(采集端发送)
    int           m_iTlvLen[MAX_TLV_FLAGS+1];
};


#endif
