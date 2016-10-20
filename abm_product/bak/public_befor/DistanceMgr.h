#ifndef     _DISTANCEMGR_H_
#define     _DISTANCEMGR_H_

#include <map>

using namespace std;

#ifndef PRIVATE_MEMORY
class DistanceType
{
public:
    int m_iDistanceTypeID;
    int m_iLongTypeID;
    int m_iRoamTypeID;
};
#endif

class CDistanceMgr
{
public:
    ~CDistanceMgr(void);

    bool GetRoamTypeByDistance(int iDistance,int& iRoamTypeID);
    bool GetLongTypeByDistance(int iDistance,int& iLongTypeID);
    static CDistanceMgr* GetInstance(void);
private:
    static CDistanceMgr* m_gDistanceMgr;
    bool Init(void);
    CDistanceMgr(void);

    bool LoadTable(const char* pTableName);
    bool UnLoad(void);

    map<int,int> m_mapDistance2RoamType;
    map<int,int> m_mapDistance2LongType;

    map<int,int>::iterator m_itPos;
    map<int,int>::const_iterator m_itBeginRoamID;
    map<int,int>::const_iterator m_itEndRoamID;

    map<int,int>::const_iterator m_itBeginLongTypeID;
    map<int,int>::const_iterator m_itEndLongTypeID;
};

#endif


