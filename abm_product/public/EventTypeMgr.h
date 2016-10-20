/*VER: 1*/ 
#ifndef EVENTTYPEMGR_H_HEADER_INCLUDED_BDAAF933
#define EVENTTYPEMGR_H_HEADER_INCLUDED_BDAAF933

#include "CommonMacro.h"
#include "KeyTree.h"
#include "HashList.h"
#include "SimpleMatrix.h"

class EventType;

class EventTypeGroup
{
public:
    int m_iGroup;
    int m_iEventType;
    unsigned int m_iNextOffset;
};

//##ModelId=4254FF490168
class EventTypeMgr
{
  public:
    //##ModelId=4255010801C5
    EventTypeMgr();

    //##ModelId=425501110377
    void load();

    //##ModelId=425501160233
    void unload();

    //##ModelId=4255011B0212
    bool getBelong(int iEventChild, int iEventParent);

    bool getBelongGroup(int iEventTypeID, int iEventTypeGroupID);

    int * getEventTypes(int iEventTypeGroupID, int * number = 0);

    //##ModelId=4255013501B6
    int getParent(int iEventChild);

    //##ModelId=4281CF3F0196
    //##Documentation
    //## 没有的话，返回-1
    int getTableType(int iEventType);

  private:
    void loadGroup();

    static SimpleMatrix * m_poMatrix;

    static HashList<int> * m_poTypeSerial;

    static HashList<int> * m_poGroupSerial;

    //##ModelId=425500C501F1
    static bool m_bUploaded;

    //##ModelId=42550066004F
    static KeyTree<EventType *> *m_poIndex;

    //##ModelId=425500980051
    static EventType *m_poEventTypeList;

    static int m_iCount;

    int m_aoEventTypeID[MAX_EVENTTYPE_PERGROUP];

};

#endif /* EVENTTYPEMGR_H_HEADER_INCLUDED_BDAAF933 */
