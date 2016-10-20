/*VER: 2*/ 
#include "CRMControl.h"
#include "Environment.h"
#include "Log.h"
#include "DBTableCheck.h"
#include "CommonMacro.h"

CRMControl::CRMControl() : m_bLocked (false)
{
	m_poHistory = 0;
	m_bPartition = DBTableCheck::checkTabColumnExist("USER_INFO_OPLOG","PARTITION_ID");
}

#ifdef CUST_INTERFACE_SERVICE_MODE

bool CRMControl::lock(bool bBlock)
{
    if (m_bLocked) return true;

    DEFINE_QUERY (qry);
    qry.setSQL ("select state from c2b_app_interface"
                " where app_id=1000");
    qry.open ();

    if (!qry.next ()) {
        Log::log (0, "c2b_app_interface记录不正常");
        THROW (1000);
    }

    strcpy (m_sState, qry.field (0).asString ());
    qry.close ();

    qry.setSQL ("update c2b_app_interface "
                "set state='F0X' where app_id=1000");
    qry.execute ();
    qry.commit ();  qry.close ();

    m_bLocked = true;

    Log::log (0, "等待CRM接口进程[0]...");
    sleep (5);

    return m_bLocked;
}

bool CRMControl::unlock()
{
    DEFINE_QUERY (qry);
    char sSQL[2048];

    if (!m_bLocked) return true;

    sprintf (sSQL, "update c2b_app_interface "
                    "set state='%s' where app_id=1000", m_sState);
    qry.setSQL (sSQL); qry.execute (); qry.commit (); qry.close ();

    m_bLocked = false;
    
    return true;
}

#else

bool CRMControl::lock(bool bBlock)
{
	if (m_bLocked) return true;

	DEFINE_QUERY (qry);
	int iCount;
	char sSql[1024];
	memset(sSql,0,sizeof(sSql));

	do {
		if (m_poHistory) {
			delete m_poHistory; m_poHistory = 0;
		}

		m_poHistory = new HashList<int> (5);

		qry.setSQL ("select to_number(run_flag), proc_id from tif_proc_run_ctl");
		qry.open ();
		iCount = 0;

		while (qry.next ()) {
			m_poHistory->add (qry.field (1).asInteger (), qry.field (0).asInteger ());
			if (qry.field (0).asInteger ()!=0 && qry.field (0).asInteger ()!=2) {
				iCount++;
			}
		}

		qry.close ();

		if (!iCount) {
				
			qry.setSQL ("update tif_proc_run_ctl set run_flag=3, last_date=sysdate"
					" where run_flag in (0, 2)");
			qry.execute (); qry.close ();

			qry.setSQL ("select count(*) from tif_proc_run_ctl where run_flag<>3");
			qry.open (); qry.next ();
			iCount = qry.field (0).asInteger ();
			qry.close ();

			if (!iCount) {
				qry.close (); qry.commit (); 

				while (!m_bLocked) {
					if (m_bPartition)
					{
						sprintf (sSql, "select count(*) from user_info_oplog where "
							"partition_id = %d "
							"and table_name<>'SERV' "
							"and table_name<>'SERV_STATE_ATTR'", UIO_PARTITION_ID_INIT);
						qry.setSQL(sSql);
					}else
					{
						qry.setSQL ("select count(*) from user_info_oplog where "
							"memop_date is null "
							"and table_name<>'SERV' "
							"and table_name<>'SERV_STATE_ATTR'");//wangs20090427 消除销帐复机对加载的影响
					}
					qry.open (); qry.next ();
					iCount = qry.field (0).asInteger ();
					qry.close ();

					sleep (2);

					if (m_bPartition)
					{
						sprintf (sSql, "select count(*) from user_info_oplog where "
							"partition_id = %d "
							"and table_name<>'SERV' "
							"and table_name<>'SERV_STATE_ATTR'", UIO_PARTITION_ID_INIT);
						qry.setSQL(sSql);
					}else
					{
						qry.setSQL ("select count(*) from user_info_oplog where "
							"memop_date is null "
							"and table_name<>'SERV' "
							"and table_name<>'SERV_STATE_ATTR'");
					}
					qry.open (); qry.next ();
					if (iCount == qry.field (0).asInteger ()) {
						m_bLocked = true;
					} else {
						Log::log (0, "等待CRM接口进程退出[1]...");
						sleep (1);
					}
					
					qry.close ();
				}

			} else {
				qry.close (); qry.rollback ();
			}

		}

		if (!m_bLocked) {
			Log::log (0, "等待CRM接口进程退出[2]...");
			sleep (2);
		}

	} while (bBlock && !m_bLocked);
	
	return m_bLocked;
}

bool CRMControl::unlock()
{
	if (m_bLocked) {
		DEFINE_QUERY (qry);
		DEFINE_QUERY (qry1);

		int iProcID, iRunFlag;
		char sSQL[2048];

		qry.setSQL ("select proc_id from tif_proc_run_ctl where run_flag=3");
		qry.open ();

		while (qry.next ()) {
			iProcID = qry.field (0).asInteger ();
			if (!m_poHistory->get (iProcID, &iRunFlag)) {
				iRunFlag = 2;
			}

			sprintf (sSQL, "update tif_proc_run_ctl set run_flag=%d, last_date=sysdate"
				" where proc_id=%d", iRunFlag, iProcID);
			qry1.setSQL (sSQL); qry1.execute (); qry1.close ();

		}

		qry.close ();	qry1.commit ();

		m_bLocked = false;
	}

	return true;
}

#endif

CRMControl::~CRMControl()
{
	while (m_bLocked) {
		unlock ();
	}
}

