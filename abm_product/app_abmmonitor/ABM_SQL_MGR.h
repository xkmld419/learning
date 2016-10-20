#ifndef __ABM_SQL_MGR_H__
#define __ABM_SQL_MGR_H__
//#include "Timesten_Env.h"
#include "TimesTenInterface.h"

//优先级
extern "C"
{
class ProfileSqlMgr
{
	public:
		static TimesTenCMD *m_pSelect;	//非锁表查询
	public:
		static void Initialize()
		{
			m_pSelect = new TimesTenCMD (TimesTenEnv::getDBConn());
			m_pSelect->Prepare(" select substr(CONDITION_VAL_ID, 5) DestParam  \
           								from abm_logic_statement b, abm_condition a \
          								where a.system_id = :systemId  \
            							and a.condition_id = b.condition_id \
            							 ");
		}
		
		static void Terminate()
		{
			if(m_pSelect != NULL)
			{
				delete m_pSelect;
				m_pSelect = NULL;
			}
		}
};


//鉴权
class ACLMonitorSqlMgr
{
	public:
		static TimesTenCMD *m_pSelect;	//非锁表查询
	public:
		static void Initialize()
		{
			m_pSelect = new TimesTenCMD (TimesTenEnv::getDBConn());
			m_pSelect->Prepare("select B.NAME,F.PRIVILEGE_TYPE  \
          from atom_service_comp A, \
               entity            B, \
               version_entity    D, \
               entity            E,  \
               entity_role       F   \
         where E.service_name = :ServiceName \
           AND A.atom_service_entity_id=E.entity_id \
           AND A.service_entity_id = B.entity_id  \
           AND D.entity_id = E.entity_id  \
           AND F.entity_id = E.entity_id  \
           AND F.system_id = :SystemId  \
           AND D.version = :Version \
           AND A.State = 'S0A' \
           AND D.STATE = 'V0A' \
           AND F.State = 'V0A' \
           ");
		}
		
		static void Terminate()
		{
			if(m_pSelect != NULL)
			{
				delete m_pSelect;
				m_pSelect = NULL;
			}
		}
};

//查找错误结果
class FindErrorSqlMgr
{
	public:
		static TimesTenCMD *m_pSelect;	//非锁表查询
	public:
		static void Initialize()
		{
			m_pSelect = new TimesTenCMD (TimesTenEnv::getDBConn());
			m_pSelect->Prepare(" select result_remart \
             from result_desc \
            where result_code = :resultCode \
            ");
		}
		
		static void Terminate()
		{
			if(m_pSelect != NULL)
			{
				delete m_pSelect;
				m_pSelect = NULL;
			}
		}
};

//取CPU和内存信息
class GetSysInfoSqlMgr
{
	public:
		static TimesTenCMD *m_pSelect;	//非锁表查询
	public:
		static void Initialize()
		{
			m_pSelect = new TimesTenCMD (TimesTenEnv::getDBConn());
			m_pSelect->Prepare("select CPU_VALUE from system_env where serve_id=1");
		}
		
		static void Terminate()
		{
			if(m_pSelect != NULL)
			{
				delete m_pSelect;
				m_pSelect = NULL;
			}
		}
};
}


#endif
