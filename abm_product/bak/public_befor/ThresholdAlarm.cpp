#include "ThresholdAlarm.h"

#include "Environment.h"

ThresholdAlarm::ThresholdAlarm(){
    m_poThresholdIndex = NULL;

    load();
}

ThresholdAlarm::~ThresholdAlarm(){
    if (m_poThresholdIndex) {
        delete m_poThresholdIndex;
        m_poThresholdIndex = NULL;
    }
}


void ThresholdAlarm::load(){
    DEFINE_QUERY(qry);

    m_poThresholdIndex = new HashList<long>(16);

    if ( !m_poThresholdIndex ) {
        Log::log(0, "[ %s ]申请m_poThresholdIndex内存失败!", __FUNCTION__);
        THROW(1);
    }

    qry.setSQL("SELECT source_event_type_id, NVL(threshold, 100000) "
               " FROM b_alarm_value_config ");
    qry.open();
    while ( qry.next() ) {
        m_poThresholdIndex->add(qry.field(0).asInteger(),
                                qry.field(1).asLong() );
    }
    qry.close();
}

bool ThresholdAlarm::getThreshold(int iSourceEventTypeID, long &lValue){
    if ( m_poThresholdIndex->get(iSourceEventTypeID, &lValue) ) {
        if (lValue < BASELINE) {
            Log::log(0, "源事件类型[ %d ]的告警阀值是 %ld ，忽略告警！",
                     iSourceEventTypeID, lValue);
            return false;
        }
        return true;
    }

    return false;
}

void ThresholdAlarm::insertLog(long lServID, const char *sAccNbr, 
                               long lEventID, long lCurValue, 
                               long lThreshold, int iBillingCycleID){
    DEFINE_QUERY(qry);

    char sSQL[1024] = {0};

    sprintf(sSQL, "INSERT INTO b_alarm_value_log(serv_id, acc_nbr, event_id, cur_value,"
                  " threshold, billing_cycle_id, state_date) "
                  " VALUES( %ld, '%s', %ld, %ld, %ld, %d, sysdate)",
             lServID, sAccNbr, lEventID, lCurValue, lThreshold, iBillingCycleID);
    qry.setSQL(sSQL);
    qry.execute();
    qry.commit();
    qry.close();
}


