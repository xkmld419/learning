/*
*   Version js at 2010-11-11
*/

#ifndef __ACCU_TT_STRUCT_H_INCLUDED_
#define __ACCU_TT_STRUCT_H_INCLUDED_

struct AccuTTStruct
{
    int m_iSize;
    int m_iDBNum;
    union {
        bool m_bSwitch[8];
        long m_lSwitch;
    } m_uStat; 
    
    void ttClose(long val) {
        m_uStat.m_lSwitch = val;
    }   

    int ttopen(int dbno) {
        return setValue(dbno, false);
    }

    int setValue(int dbno, bool status) {
        if ((dbno<0) || (dbno>=7))   //目前只支持7个库将来改成按位
            return -1;
        m_uStat.m_bSwitch[dbno] = status;
        return 0;
    }

    bool ttValid() {
        return !m_uStat.m_lSwitch;
    }

    bool ttUnvalid(int dbno) {
        if ((dbno<0) || (dbno>=7))
            return false;
        return m_uStat.m_bSwitch[dbno];
    }
    
    long getState() {
        return m_uStat.m_lSwitch;
    }
};

#endif /*__ACCU_TT_STRUCT_H_INCLUDED_*/

