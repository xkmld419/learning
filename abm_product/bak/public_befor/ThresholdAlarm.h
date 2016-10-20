
#ifndef _THRESH_HOLD_ALARM_H_
#define _THRESH_HOLD_ALARM_H_

#include "HashList.h"

/**
 * 2010-08-26 
 * 大额清单费用告警 
 */

const int BASELINE = 100000;

class ThresholdAlarm {

    void load();
    HashList<long> *m_poThresholdIndex;

public:
    ThresholdAlarm();
    ~ThresholdAlarm();

    bool getThreshold(int iSourceTypeID, long &lValue);

    void insertLog(long lServID, const char *sAccNbr, 
                   long lEventID, long lCurValue, 
                   long lThreshold, int iBillingCycleID);

};

#endif //_THRESH_HOLD_ALARM_H_



