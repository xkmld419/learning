/*VER: 1*/ 
#include "EventTypeMgr.h"
#include "EventType.h"
#include "Environment.h"
#include "TOCIQuery.h"
#include "Exception.h"
#include "MBC.h"
#ifndef PRIVATE_MEMORY
#include "ParamInfoInterface.h"
#endif

//##ModelId=4255010801C5
#ifdef PRIVATE_MEMORY
EventTypeMgr::EventTypeMgr()
{
	if (!m_bUploaded) {
		load ();
		loadGroup ();
	}
}
#else
EventTypeMgr::EventTypeMgr()
{
}
#endif
//##ModelId=425501110377
void EventTypeMgr::load()
{
	unload ();

	TOCIQuery qry (Environment::getDBConn ());
	int count, i;
	EventType *pEventType;


	m_poIndex = new KeyTree<EventType *> ();
	if (!m_poIndex) THROW(MBC_EventTypeMgr+1);

	qry.setSQL ("select count(*) from ratable_event_type");
	qry.open (); qry.next ();
	count = qry.field(0).asInteger () + 16;	

	qry.close ();

	m_poEventTypeList = new EventType[count];
	if (!m_poEventTypeList) THROW(MBC_EventTypeMgr+1);

	m_poTypeSerial = new HashList<int> (count);
	if (!m_poTypeSerial) THROW(MBC_EventTypeMgr+1);

	qry.setSQL ("select event_type_id child_type, nvl(sum_event_type, -1) parent_type, "
		"nvl(event_table_type_id, -1) table_type "
			"from ratable_event_type where state='10A'");

	qry.open ();

	i = 0;

	while (qry.next ()) {

		if (i == count) THROW(MBC_EventTypeMgr+2);

		pEventType = m_poEventTypeList + i;
		
		pEventType->m_iEventType = qry.field(0).asInteger ();
		pEventType->m_iParentType = qry.field(1).asInteger ();
		pEventType->m_iTableType = qry.field(2).asInteger ();

		m_poIndex->add (pEventType->m_iEventType, pEventType);
		m_poTypeSerial->add (pEventType->m_iEventType, i);

		i++;
	}

	qry.close ();

	count = i;
	m_iCount = i;

	for (i=0; i<count; i++) {
		pEventType = m_poEventTypeList + i;
		
		if (pEventType->m_iParentType != -1) {
			m_poIndex->get (pEventType->m_iParentType, &(pEventType->m_poParent));
		}
	}

	m_bUploaded = true;

}

#ifdef PRIVATE_MEMORY
bool EventTypeMgr::getBelongGroup(int iEventTypeID, int iGroupID)
{
	// add a NULL pointer estimation.  20050826 chenk
        int k, l;
        if ( m_poTypeSerial != NULL && m_poGroupSerial != NULL )
        {
        	if (!(m_poTypeSerial->get(iEventTypeID, &k)) ||
                !(m_poGroupSerial->get(iGroupID, &l)) )
                return false;
        }
        
        return m_poMatrix == NULL ? false : m_poMatrix->getBelong (k, l);
}
#else
bool EventTypeMgr::getBelongGroup(int iEventTypeID, int iGroupID)
{
    EventTypeGroup *pEventTypeGroup = 0;
    EventTypeGroup *pHead = 0;
    unsigned int iIdx = 0;
    int i = 0;

    pHead = (EventTypeGroup *)G_PPARAMINFO->m_poEventTypeGroupList;
    if(G_PPARAMINFO->m_poEventTypeGroupIndex->get(iGroupID, &iIdx))
    {
        pEventTypeGroup = pHead + iIdx;
        while(pEventTypeGroup != pHead)
        {
            if(pEventTypeGroup->m_iEventType == iEventTypeID)
                return true;
            else
            {
                pEventTypeGroup = pHead + pEventTypeGroup->m_iNextOffset;
            }
        }
        return false;
    }
    else
    {
        return false;
    }
}
#endif

#ifdef PRIVATE_MEMORY
int * EventTypeMgr::getEventTypes(int iGroupID, int * number)
{
	memset (m_aoEventTypeID, 0, sizeof (m_aoEventTypeID));

        int k;
        int *pInt ;
        int i = 0;

        if (m_poGroupSerial->get (iGroupID, &k)) {
                pInt = m_poMatrix->getChilds (k);
                for (i=0; pInt[i]!=-1; i++) {
                        m_aoEventTypeID[i] = m_poEventTypeList[pInt[i]].m_iEventType;
                }
        }

        if (number) *number = i;

        m_aoEventTypeID[i] = -1;

        return m_aoEventTypeID;
}
#else
int * EventTypeMgr::getEventTypes(int iGroupID, int * number)
{
	memset (m_aoEventTypeID, 0, sizeof (m_aoEventTypeID));

    EventTypeGroup *pEventTypeGroup = 0;
    EventTypeGroup *pHead = 0;
    unsigned int iIdx = 0;
    int i = 0;

    pHead = (EventTypeGroup *)G_PPARAMINFO->m_poEventTypeGroupList;
    if(G_PPARAMINFO->m_poEventTypeGroupIndex->get(iGroupID, &iIdx))
    {
        pEventTypeGroup = pHead + iIdx;
        while(pEventTypeGroup != pHead)
        {
            m_aoEventTypeID[i] = pEventTypeGroup->m_iEventType;
            i++;
            pEventTypeGroup = pHead + pEventTypeGroup->m_iNextOffset;
        }
        if(number)
            *number = i;
        m_aoEventTypeID[i] = -1;
        return m_aoEventTypeID;
    }
    else
    {
        if(number)
            *number = i;
        m_aoEventTypeID[i] = -1;
        return m_aoEventTypeID;
    }
}
#endif

void EventTypeMgr::loadGroup()
{
	int i, j, k, l;
        int curGroup = 0;
        char sql[2048];

	DEFINE_QUERY (qry);

        strcpy (sql, "select count(distinct type_group_id) from b_event_type_group_member");
        qry.setSQL (sql); qry.open (); qry.next ();
        j = qry.field(0).asInteger ();
        qry.close ();

        m_poGroupSerial = new HashList<int> ((j>>1) | 1);
        m_poMatrix = new SimpleMatrix (m_iCount, j, false);

        if (!m_poGroupSerial || !m_poMatrix)
                THROW(MBC_EventTypeMgr+1);

		memset(sql,0,sizeof(sql));
        sprintf (sql, "select event_type_id, type_group_id from b_event_type_group_member");
        qry.setSQL (sql); qry.open ();

        while (qry.next ()) {
                i = qry.field(0).asInteger ();
                j = qry.field(1).asInteger ();

                if (!(m_poTypeSerial->get(i, &k))) {
                        THROW (MBC_EventTypeMgr+11);
                }

                if (!(m_poGroupSerial->get(j, &l))) {
                        l = curGroup++;
                        m_poGroupSerial->add (j, l);
				}

                m_poMatrix->setBelong (k, l);
        }
        qry.close ();
}

//##ModelId=425501160233
void EventTypeMgr::unload()
{
	m_bUploaded = false;

	if (m_poIndex) {
		delete m_poIndex;
		m_poIndex = 0;
	}

	if (m_poEventTypeList) {
		delete m_poEventTypeList;
		m_poEventTypeList = 0;
	}

	if (m_poMatrix) {
		delete m_poMatrix;
		m_poMatrix = 0;
	}

	if (m_poTypeSerial) {
		delete m_poTypeSerial;
		m_poTypeSerial = 0;
	}

}

//##ModelId=4255011B0212
#ifdef PRIVATE_MEMORY
bool EventTypeMgr::getBelong(int iEventChild, int iEventParent)
{
	EventType * pEventType = 0;

	if (!(m_poIndex->get (iEventChild, &pEventType))) return false;

	if (iEventChild == iEventParent) return true;

	while (pEventType) {
		if (pEventType->m_iParentType == iEventParent)
			return true;

		pEventType = pEventType->m_poParent;
	}

	return false;
}
#else
bool EventTypeMgr::getBelong(int iEventChild, int iEventParent)
{
    EventType *pEventType = 0;
    EventType *pHead = 0;
    unsigned int iIdx = 0;

    pHead = (EventType *)G_PPARAMINFO->m_poEventTypeList;
    if(!(G_PPARAMINFO->m_poEventTypeIndex->get(iEventChild, &iIdx)))
        return false;
    else if(iEventChild == iEventParent)
        return true;
    else
    {
        pEventType = pHead + iIdx;
        while(1)
        {
            if(pEventType->m_iParentType == iEventParent)
                return true;
            else if(pEventType->m_iParentOffset != 0)
                pEventType = pHead + pEventType->m_iParentOffset;
            else
                return false;
        }
        return false;
    }
}
#endif

//##ModelId=4255013501B6
#ifdef PRIVATE_MEMORY
int EventTypeMgr::getParent(int iEventChild)
{
	EventType * pEventType = 0;

	if (!(m_poIndex->get (iEventChild, &pEventType))) return -1;

	return pEventType->m_iParentType;
}
#else
int EventTypeMgr::getParent(int iEventChild)
{
    EventType *pEventType = 0;
    EventType *pHead = 0;
    unsigned int iIdx = 0;

    pHead = (EventType *)G_PPARAMINFO->m_poEventTypeList;
    if(G_PPARAMINFO->m_poEventTypeIndex->get(iEventChild, &iIdx))
    {
        pEventType = pHead + iIdx;
        return pEventType->m_iParentType;
    }
    else
    {
        return -1;
    }
}
#endif

//##ModelId=4281CF3F0196
#ifdef PRIVATE_MEMORY
int EventTypeMgr::getTableType(int iEventType)
{
	EventType * pEventType = 0;

	m_poIndex->get (iEventType, &pEventType);

	while (pEventType) {
		if (pEventType->m_iTableType != -1) return pEventType->m_iTableType;
		pEventType = pEventType->m_poParent;
	}

	return -1;
}
#else
int EventTypeMgr::getTableType(int iEventType)
{
	EventType *pEventType = 0;
    EventType *pHead = 0;
    unsigned int iIdx = 0;

    pHead = (EventType *)G_PPARAMINFO->m_poEventTypeList;
    if(G_PPARAMINFO->m_poEventTypeIndex->get(iEventType, &iIdx))
    {
        pEventType = pHead + iIdx;
        while(pEventType != pHead)
        {
            if(pEventType->m_iTableType != -1)
                return pEventType->m_iTableType;
            else
                pEventType = pHead + pEventType->m_iParentOffset;
        }
    }
    else
    {
        return -1;
    }
}
#endif

//##ModelId=425500C501F1
bool EventTypeMgr::m_bUploaded = false;

//##ModelId=42550066004F
KeyTree<EventType *> *EventTypeMgr::m_poIndex = 0;

//##ModelId=425500980051
EventType *EventTypeMgr::m_poEventTypeList = 0;

SimpleMatrix * EventTypeMgr::m_poMatrix = 0;

HashList<int> * EventTypeMgr::m_poTypeSerial = 0;

HashList<int> * EventTypeMgr::m_poGroupSerial = 0;

int EventTypeMgr::m_iCount = 0;

