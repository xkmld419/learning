/*VER: 1*/ 
#ifndef GROUP_INST_H_HEADER_INCLUDED
#define GROUP_INST_H_HEADER_INCLUDED

/*
* Copyright (c) 2007,
* All rights reserved.
*
* 文件名称： GroupInstMgr.h
* 文件标识：
* 摘    要： 群组实例
*
* 当前版本：1.0
* 作    者：zhangap
* 完成日期：2007年 月 日
*
*/

#include "CommonMacro.h"
#include "HashList.h"
#include "UserInfoReader.h"
#include "StdEvent.h"
#include "LifeCycleMgr.h"

class   GroupInstMember
{

public:

    long m_lGroupID;
    int  m_iGroupTypeID;
    int  m_iMemberTypeID;
    int  m_iRoleTypeID;
    int  m_iLifeCycleID;

    long m_lServID;
    long m_lServID_2;
    long m_lObjectID;
    GroupInstMember * m_pNext;

    char m_sAreaCode[13];
    char m_sAccNbr[21];
    char m_sAccNbr_2[21];
    char m_sMaxEndAccNbr[21];

};



class   GroupInstMgr
{ public:

    GroupInstMgr ( void );
    ~GroupInstMgr ( void );


    void load ( void );
    void unload ( void );

    bool getGroupInst (
             char*   sAreaCode,
             char*   sAccNbr,
             char*   sDate,
             int     iGroupType,
             vector<GroupInstMember *> &o_vecGroupInst  );
    bool getGroupInst (
             long    lServID,
             char*   sDate,
             int     iGroupType,
             vector<GroupInstMember *> &o_vecGroupInst  );

    bool ifBelong (
             char*   sAreaCode,
             char*   sAccNbr,
             char*   sDate,
             long    lGroupID );
    bool ifBelong (
             long    lServID,
             char*   sDate,
             long    lGroupID );
    bool ifExist (
             char*   sAreaCode,
             char*   sAccNbr,
             char*   sDate,
             int     iGroupType );
    bool ifExist (
             long    lServID,
             char*   sDate,
             int     iGroupType );


    bool ifSameGroup (
             long    lServID,
             long    lServID2,
             char*   sDate,
             int     iGroupType =0 );
             
    bool ifSameGroup (
             long    lServID,
             char*   sAreaCode,
             char*   sAccNbr,
             char*   sDate,
             int     iGroupType =0 );
    bool ifSameGroup (
             char*   sAreaCode,
             char*   sAccNbr,
             char*   sAreaCode2,
             char*   sAccNbr2,
             char*   sDate,
             int     iGroupType =0 );


static bool  m_bUploaded ;

static HashList<GroupInstMember *>* m_phpAccNbr;
static HashList<GroupInstMember *>* m_phpServID;
static HashList<GroupInstMember *>* m_phpNestGroup;
static GroupInstMember * m_pInstMemberList;

static GroupInstMember * m_aSegAccNbrStr;
static GroupInstMember * m_aSegAccNbrLong;
static GroupInstMember * m_aSegServID;

static int m_iSegAccNbrStr;
static int m_iSegAccNbrLong;
static int m_iSegServID;

static LifeCycleMgr * m_poLifeCycleMgr;

private:
    void loadTheSingle( );
    void loadSegAccNbrStr( );
    void loadSegAccNbrLong( );
    void loadSegServID( );

    bool getSegStr( char *_sAreaCode, char *_sAccNBR, char *_sDate,
        int iGroupType, vector<GroupInstMember *> &_vecGroupInst);
    bool getSegLong(long _longID, char * _sAreaCode,
        GroupInstMember * _aSegInst, int _iTotalCnt, char *_sDate,
        int iGroupType, vector<GroupInstMember *> &_vecGroupInst);
                
    bool checkDate(int iLifeCycle, char* sDate);

};


#endif  /*  GROUP_INST_H_HEADER_INCLUDED  */
