#include "TTimport.h"
#include "Log.h"


// TimesTenCMD *m_poPublicConn;


TTimport::TTimport()
{

}

TTimport::~TTimport()
{

}

int TTimport::qryXKMtest(TimesTenCMD *m_poPublicConn,vector<XKMtest *> &vXKMtest)
{
	int iRet=-1;
	bool bFlag = false;
    int i=0;
	try 
	{
	    /**
		string sql;

		sql +=" insert xkm_test(STATE_DATE,SERV_ID,SPLIT_BY) values  ";
		sql +=" (:p0,p) ";

		cout<<"sql:"<<sql<<endl;
		**/

		for (i=0; i<vXKMtest.size(); i++)
		{	
		    m_poPublicConn->Close();
            m_poPublicConn->Prepare("insert into xkm_test (STATE_DATE,SERV_ID,SPLIT_BY) values \
            (:p1,:p2,:p3)");
			m_poPublicConn->setParam(1, vXKMtest[i]->STATE_DATE);
			m_poPublicConn->setParam(2, vXKMtest[i]->SERV_ID);
			m_poPublicConn->setParam(3, vXKMtest[i]->SPLIT_BY);
	        m_poPublicConn->Execute();
	        m_poPublicConn->Commit();
			//i++;
            iRet = 0;
		}	

		m_poPublicConn->Close();

	}
	catch (TTStatus oSt)
	{
		Log::log(0, "表数据查询失败...");
	  return -1;
	}

	return iRet;

}

