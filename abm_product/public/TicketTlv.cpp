/*VER: 1*/ 
#include "TicketTlv.h"


// 当前TicketTlv 所有tag位数据清空
void TicketTlv::reset ()
{
    for (int i=0; i<=MAX_TLV_FLAGS; i++)
        resetField (i);
}

// 根据tag值获取相应的字段值, Tlv数据有效长度, 字段长度
// _iTag      : TAG标识
// pValue     : 返回的字段值的地址
// piTlvLen   : Tlv数据有效长度
bool TicketTlv::getTagValue (int _iTag, unsigned char *pValue, int &iTlvLen)
{
    if (_iTag<0 || _iTag>MAX_TLV_FLAGS) return false;
    
    if (!m_lAddress[_iTag] || !m_iTlvLen[_iTag]) return false;
        
    memcpy (pValue, (void *)m_lAddress[_iTag], m_iTlvLen[_iTag]);
    
    iTlvLen = m_iTlvLen[_iTag];
    
    return true;
}

// 置Tag位的值
bool TicketTlv::setTagValue (int _iTag, void *pValue, int _iTlvLen)
{
    if (_iTag<0 || _iTag>MAX_TLV_FLAGS) return false;
    
    ajustTagLen (_iTag, _iTlvLen);
     
    memcpy ((void *)m_lAddress[_iTag], pValue, _iTlvLen);
    
    m_iTlvLen[_iTag] = _iTlvLen;
    
    return true;
}


// 申请一个Tag字段节点  return: 0--OK, -1--Malloc Err, 1--Tag位不合法
int TicketTlv::mallocField (int _iTag, int _iLen) 
{
    if (_iTag<0 || _iTag>MAX_TLV_FLAGS )  return 1;
    
    if (_iLen > MAX_FIELD_LEN) return -1;
    
    m_lAddress[_iTag] = (unsigned long)malloc (_iLen+1);
    
    if (!m_lAddress[_iTag]) THROW (100100001);
    
    m_iFieldLen[_iTag] = _iLen;
    
    resetField (_iTag);
    
    return 0;
}

// 校验,调整Tag位字段, 满足实际长度(_iTlvLen)的需要
void TicketTlv::ajustTagLen (int _iTag, int _iTlvLen)
{
    if (_iTag<0 || _iTag>MAX_TLV_FLAGS) return;
    
    // 字段尚未创建
    if (!m_lAddress[_iTag]) {
        mallocField (_iTag, _iTlvLen);
        return;
    }
    
    // 检查当前字段长度是否满足实际需要
    if (m_iFieldLen[_iTag] >= _iTlvLen) return;
            
    free ((void *)m_lAddress[_iTag]);
    
    mallocField (_iTag, _iTlvLen);
}

// 指定tag位数据清空
void TicketTlv::resetField (int _iTag)
{
    if (_iTag<0 || _iTag>MAX_TLV_FLAGS) return;
    
    if (!m_lAddress[_iTag]) return;
    
    memset ((void *)m_lAddress[_iTag], 0, m_iFieldLen[_iTag]);
    
    m_iTlvLen[_iTag] = 0;
}
