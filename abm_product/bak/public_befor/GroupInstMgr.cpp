/*VER: 1*/ 

#include "GroupInstMgr.h"

////初始化变量

    bool GroupInstMgr::m_bUploaded = false;
    GroupInstMember * GroupInstMgr::m_pInstMemberList = NULL;

    HashList<GroupInstMember *>* GroupInstMgr::m_phpAccNbr = NULL;
    HashList<GroupInstMember *>* GroupInstMgr::m_phpServID = NULL;
    HashList<GroupInstMember *>* GroupInstMgr::m_phpNestGroup = NULL;

    GroupInstMember * GroupInstMgr::m_aSegAccNbrStr = NULL;
    GroupInstMember * GroupInstMgr::m_aSegAccNbrLong = NULL;
    GroupInstMember * GroupInstMgr::m_aSegServID = NULL;


    int GroupInstMgr::m_iSegAccNbrStr=0;
    int GroupInstMgr::m_iSegAccNbrLong=0;
    int GroupInstMgr::m_iSegServID=0;

    LifeCycleMgr * GroupInstMgr::m_poLifeCycleMgr = NULL;

bool GroupInstMgr::checkDate(int iLifeCycle, char* sDate)
{
    if(m_poLifeCycleMgr == NULL)
        m_poLifeCycleMgr = new LifeCycleMgr();

    LifeCycle * pCycle = m_poLifeCycleMgr->getLifeCycle(iLifeCycle);
    if(pCycle==NULL)
        return false;
    if( G_POFFERINSQO->m_poOfferIns->m_lProdOfferInsID > 0)
        return pCycle->check(sDate, G_POFFERINSQO->m_poOfferIns);
    else
        return pCycle->check(sDate);

    return false;
}

GroupInstMgr::GroupInstMgr()
{
	if (!m_bUploaded) load ();
}

GroupInstMgr::~GroupInstMgr()
{
	unload ();
}



void GroupInstMgr::unload()
{
	m_bUploaded = false;
	if (m_phpAccNbr) {
		delete m_phpAccNbr;
		m_phpAccNbr = NULL;
	}
	if (m_phpServID) {
		delete m_phpServID;
		m_phpServID = NULL;
	}
	if (m_pInstMemberList) {
		delete [] m_pInstMemberList;
		m_pInstMemberList = NULL;
	}


	if (m_aSegAccNbrStr) {
		delete [] m_aSegAccNbrStr;
		m_aSegAccNbrStr = NULL;
	}
	if (m_aSegAccNbrLong) {
		delete [] m_aSegAccNbrLong;
		m_aSegAccNbrLong = NULL;
	}
	if (m_aSegServID) {
		delete [] m_aSegServID;
		m_aSegServID = NULL;
	}


}


void GroupInstMgr::load()
{
    unload ();
    loadTheSingle();
    loadSegAccNbrStr();
    loadSegAccNbrLong();
    loadSegServID();
    m_bUploaded=true;
}


////初始化数据: servID, 号码, 嵌套groupID
void GroupInstMgr::loadTheSingle()
{
	int i=0;
	GroupInstMember * pInstMember=NULL;
	GroupInstMember * pInstMember_get=NULL;

	TOCIQuery qry (Environment::getDBConn ());

	qry.setSQL ("select count(*) from group_instance_member"
	    " where member_type_id in (1,3,6)");
	qry.open (); qry.next ();

	i = qry.field(0).asInteger () + 16;

	qry.close ();

	m_pInstMemberList = new GroupInstMember[i];
	if (!m_pInstMemberList) THROW(MBC_LifeCycleMgr+1);

    memset(m_pInstMemberList, 0, sizeof(GroupInstMember)*i );

	m_phpAccNbr = new HashList<GroupInstMember *> ((i>>1) + 16);
	m_phpServID = new HashList<GroupInstMember *> ((i>>1) + 16);
	m_phpNestGroup = new HashList<GroupInstMember *> ((i>>1) + 16);

	if (!m_phpServID || !m_phpAccNbr || !m_phpNestGroup)
	    THROW(MBC_LifeCycleMgr+1);

	qry.setSQL(
        " select a.group_id, a.group_type, b.member_type_id,b.member_role_id,"
        " b.serv_id, b.serv_id_2, b.area_code, b.acc_nbr, b.acc_nbr_2,"
        " b.life_cycle_id, b.member_object_id "
        " from group_instance a ,group_instance_member b "
        " where a.group_id = b.group_id "
        " and b.member_type_id in (1,3,6)"
	    );
	qry.open();
    while(qry.next()){
        i--;
        if (i<0) THROW(MBC_LifeCycleMgr+2);
        pInstMember = &m_pInstMemberList[i];

        pInstMember->m_lGroupID=qry.field("group_id").asLong();
        pInstMember->m_iGroupTypeID=qry.field("group_type").asInteger();
        pInstMember->m_iMemberTypeID=qry.field("member_type_id").asInteger();
        pInstMember->m_iRoleTypeID=qry.field("member_role_id").asInteger();
        pInstMember->m_iLifeCycleID=qry.field("life_cycle_id").asInteger();

        switch(pInstMember->m_iMemberTypeID){
            case 1:
                //servID
                pInstMember->m_lServID=qry.field("serv_id").asLong();
                if(m_phpServID->get(pInstMember->m_lServID, &pInstMember_get)){
                    while(pInstMember_get->m_pNext !=NULL){
                        pInstMember_get=pInstMember_get->m_pNext;
                    }
                    pInstMember_get->m_pNext=pInstMember;
                }else{
                    m_phpServID->add(pInstMember->m_lServID, pInstMember);
                }
                break;

            case 3:
                //accNBR
                strncpy(pInstMember->m_sAreaCode,
                    qry.field("area_code").asString(),
                    sizeof(pInstMember->m_sAreaCode) );

                strncpy(pInstMember->m_sAccNbr,
                    qry.field("acc_nbr").asString(),
                    sizeof(pInstMember->m_sAccNbr) );

                if(m_phpAccNbr->get(pInstMember->m_sAccNbr, &pInstMember_get)){
                    while(pInstMember_get->m_pNext !=NULL){
                        pInstMember_get=pInstMember_get->m_pNext;
                    }
                    pInstMember_get->m_pNext=pInstMember;
                }else{
                    m_phpAccNbr->add(pInstMember->m_sAccNbr, pInstMember);
                }

                break;

            case 6:
                //groupID
                pInstMember->m_lObjectID=qry.field("member_object_id").asLong();

                if(m_phpNestGroup->get(pInstMember->m_lObjectID, &pInstMember_get)){
                    while(pInstMember_get->m_pNext !=NULL){
                        pInstMember_get=pInstMember_get->m_pNext;
                    }
                    pInstMember_get->m_pNext=pInstMember;
                }else{
                    m_phpNestGroup->add(pInstMember->m_lObjectID, pInstMember);
                }
                break;

            default :
                break;
            }

    }

}



////初始化数据: 号码段(字符串)
void GroupInstMgr::loadSegAccNbrStr()
{

	GroupInstMember * pInstMember=NULL;

	char sMaxEndNBRAhead[16]={0};

	TOCIQuery qry (Environment::getDBConn ());
    qry.close ();

	qry.setSQL ("select count(*) cnt from group_instance_member"
	    " where member_type_id=4");
    qry.open();
    qry.next();


    m_iSegAccNbrStr = qry.field("cnt").asInteger();
    if ( m_iSegAccNbrStr == 0 ) return;

    m_aSegAccNbrStr = new GroupInstMember[m_iSegAccNbrStr];
    if (!m_aSegAccNbrStr)
        THROW(MBC_FormatMethod+8);
    memset(m_aSegAccNbrStr, 0, sizeof(GroupInstMember)*m_iSegAccNbrStr );

    qry.close();
    qry.setSQL(
        " select a.group_id, a.group_type, b.member_type_id,b.member_role_id,"
        " b.area_code, b.acc_nbr, b.acc_nbr_2,"
        " b.life_cycle_id, b.member_object_id "
        " from group_instance a ,group_instance_member b "
        " where a.group_id = b.group_id "
        " and b.member_type_id =4"
        " order by acc_nbr,acc_nbr_2"
        );
    qry.open();

    // 按 begin_nbr 从小到大字符串排序, 放入数组 m_aSegAccNbrStr 中
    for (int iPos=0; qry.next() && (iPos<m_iSegAccNbrStr); iPos++) {
        pInstMember = &m_aSegAccNbrStr[iPos];
        pInstMember->m_lGroupID=qry.field("group_id").asLong();
        pInstMember->m_iGroupTypeID=qry.field("group_type").asInteger();
        pInstMember->m_iMemberTypeID=qry.field("member_type_id").asInteger();
        pInstMember->m_iRoleTypeID=qry.field("member_role_id").asInteger();
        pInstMember->m_iLifeCycleID=qry.field("life_cycle_id").asInteger();
        strncpy(pInstMember->m_sAreaCode,
            qry.field("area_code").asString(),
            sizeof(pInstMember->m_sAreaCode) );

        strncpy(pInstMember->m_sAccNbr,
            qry.field("acc_nbr").asString(),
            sizeof(pInstMember->m_sAccNbr) );

        strncpy(pInstMember->m_sAccNbr_2,
            qry.field("acc_nbr_2").asString(),
            sizeof(pInstMember->m_sAccNbr_2) );

        strncpy(pInstMember->m_sMaxEndAccNbr,
            sMaxEndNBRAhead,
            sizeof(pInstMember->m_sMaxEndAccNbr) );


        if (strcmp(pInstMember->m_sAccNbr_2, sMaxEndNBRAhead)>0)
            strncpy(sMaxEndNBRAhead,
                pInstMember->m_sAccNbr_2,
                sizeof(sMaxEndNBRAhead) );

    }

    qry.close();

}



////初始化数据: 号码段(整数)
void GroupInstMgr::loadSegAccNbrLong()
{

	GroupInstMember * pInstMember=NULL;
	long lMaxEnd=0;

	TOCIQuery qry (Environment::getDBConn ());
    qry.close ();

	qry.setSQL ("select count(*) cnt from group_instance_member"
	    " where member_type_id = 5");
    qry.open();
    qry.next();

    m_iSegAccNbrLong = qry.field("cnt").asInteger();
    if ( m_iSegAccNbrLong == 0 ) return;

    m_aSegAccNbrLong = new GroupInstMember[m_iSegAccNbrLong];
    if (!m_aSegAccNbrLong)
        THROW(MBC_FormatMethod+8);
    memset(m_aSegAccNbrLong, 0, sizeof(GroupInstMember)*m_iSegAccNbrLong );

    qry.close();
    qry.setSQL(
        " select a.group_id, a.group_type, b.member_type_id,b.member_role_id,"
        " b.area_code, to_number(nvl(b.acc_nbr,'0')) acc_nbr,"
        " to_number(nvl(b.acc_nbr_2,'0')) acc_nbr_2,"
        " b.life_cycle_id, b.member_object_id "
        " from group_instance a ,group_instance_member b "
        " where a.group_id = b.group_id "
        " and b.member_type_id=5"
        " order by to_number(nvl(b.acc_nbr,'0')),"
        " to_number(nvl(b.acc_nbr_2,'0'))"
        );
    qry.open();

    // 按 to_number(nvl(b.acc_nbr,'0')) 从小到大字符串排序,
    // 放入数组 m_aSegAccNbrLong 中
    for (int iPos=0; qry.next() && (iPos<m_iSegAccNbrLong); iPos++) {
        pInstMember = &m_aSegAccNbrLong[iPos];
        pInstMember->m_lGroupID=qry.field("group_id").asLong();
        pInstMember->m_iGroupTypeID=qry.field("group_type").asInteger();
        pInstMember->m_iMemberTypeID=qry.field("member_type_id").asInteger();
        pInstMember->m_iRoleTypeID=qry.field("member_role_id").asInteger();
        pInstMember->m_iLifeCycleID=qry.field("life_cycle_id").asInteger();

        strncpy(pInstMember->m_sAreaCode,
            qry.field("area_code").asString(),
            sizeof(pInstMember->m_sAreaCode) );

        /// 请注意: acc_nbr转换成long型,是放在servID这个字段上了
        /// m_lObjectID 也用来复用了, 存放 maxend 值
        pInstMember->m_lServID=qry.field("acc_nbr").asLong();
        pInstMember->m_lServID_2=qry.field("acc_nbr_2").asLong();
        pInstMember->m_lObjectID=lMaxEnd;


        if( (pInstMember->m_lServID) > lMaxEnd){
            lMaxEnd = pInstMember->m_lServID;
        }

    }

    qry.close();

}




////初始化 servID 段
void GroupInstMgr::loadSegServID()
{

	GroupInstMember * pInstMember=NULL;
	long lMaxEnd=0;

	TOCIQuery qry (Environment::getDBConn ());
    qry.close ();

	qry.setSQL ("select count(*) cnt from group_instance_member"
	    " where member_type_id = 2");
    qry.open();
    qry.next();

    m_iSegServID = qry.field("cnt").asInteger();
    if ( m_iSegServID == 0 ) return;

    m_aSegServID = new GroupInstMember[m_iSegServID];
    if (!m_aSegServID)
        THROW(MBC_FormatMethod+8);
    memset(m_aSegServID, 0, sizeof(GroupInstMember)*m_iSegServID );

    qry.close();
    qry.setSQL(
        " select a.group_id, a.group_type, b.member_type_id,b.member_role_id,"
        " b.area_code, b.serv_id, b.serv_id_2,"
        " b.life_cycle_id, b.member_object_id "
        " from group_instance a ,group_instance_member b "
        " where a.group_id = b.group_id "
        " and b.member_type_id=2"
        " order b.serv_id, b.serv_id_2"
        );
    qry.open();

    // 按 serv_id  从小到大字符串排序,
    // 放入数组 m_aSegServID 中
    for (int iPos=0; qry.next() && (iPos<m_iSegServID); iPos++) {
        pInstMember = &m_aSegServID[iPos];
        pInstMember->m_lGroupID=qry.field("group_id").asLong();
        pInstMember->m_iGroupTypeID=qry.field("group_type").asInteger();
        pInstMember->m_iMemberTypeID=qry.field("member_type_id").asInteger();
        pInstMember->m_iRoleTypeID=qry.field("member_role_id").asInteger();
        pInstMember->m_iLifeCycleID=qry.field("life_cycle_id").asInteger();

        /// m_lObjectID 用来复用了, 存放 maxend 值
        pInstMember->m_lServID=qry.field("serv_id").asLong();
        pInstMember->m_lServID_2=qry.field("serv_id_2").asLong();
        pInstMember->m_lObjectID=lMaxEnd;

        if( (pInstMember->m_lServID) > lMaxEnd){
            lMaxEnd = pInstMember->m_lServID;
        }

    }

    qry.close();

}



///////////////////////////////////////////////////////////
////查找段数据来获取信息

bool GroupInstMgr::getSegStr( char *_sAreaCode, char *_sAccNBR,
    char *_sDate, int iGroupType, vector<GroupInstMember *> &_vecGroupInst)
{

    int iHead = 0, iTail = m_iSegAccNbrStr-1;
    int iMid ;
    if (m_iSegAccNbrStr == 0) return false;
    // 折半查找, 试图在m_aSegAccNbrStr中找到一个 m_sBeginNBR == _sAccNBR 的记录
    // 可能的结果:
    //     1. 找到, iMid 指向 一个 m_sBeginNBR == _sAccNBR 的记录
    //     2. 未找到, 可能情况:
    //          a. iMid 指向最后一个 m_sBeginNBR < _sAccNBR 的记录
    //          b. iMid 指向最前一个 m_sBeginNBR > _sAccNBR 的记录
    while (iHead <= iTail) {
        iMid = (iHead + iTail)/2;
        int iRet = strcmp(_sAccNBR, m_aSegAccNbrStr[iMid].m_sAccNbr);
        if (iRet > 0) {
            iHead = iMid+1;
            continue;
        }
        if (iRet < 0) {
            iTail = iMid-1;
            continue;
        }
        //found:
        break;
    }

    //目标: 让iMid 指向 最后一个 (m_sBeginNBR <= _sAccNBR) 的 m_aPNSegBilling 记录.
    int iRet = strcmp(_sAccNBR, m_aSegAccNbrStr[iMid].m_sAccNbr);
    if (iRet < 0) {
        iMid--;
    }
    else if (iRet == 0) { //游标试图向后<增大>查找是否有与当前m_sBeginNBR相同的记录
        while (iMid<m_iSegAccNbrStr-1) {
            iMid++;
            if (strcmp(_sAccNBR, m_aSegAccNbrStr[iMid].m_sAccNbr)<0) {
                iMid--;
                break;
            }
        }
    }

    // 向前遍历匹配
    for ( ; iMid>=0 ; iMid--) {

        int iRetA = strcmp(_sAccNBR, m_aSegAccNbrStr[iMid].m_sAccNbr_2);
        if (iRetA <= 0) { // _sAccNBR 在当前号段内
            if(strcmp(_sAreaCode, m_aSegAccNbrStr[iMid].m_sAreaCode)!=0)
                continue;
            if((iGroupType<=0||m_aSegAccNbrStr[iMid].m_iGroupTypeID==iGroupType)
                && checkDate(m_aSegAccNbrStr[iMid].m_iLifeCycleID, _sDate) )
                _vecGroupInst.push_back( &m_aSegAccNbrStr[iMid] );

        }
        // 判断向前是否还有 m_sEndNBR 不比 _sAccNBR 小的记录
        if (strcmp(_sAccNBR, m_aSegAccNbrStr[iMid].m_sMaxEndAccNbr)>0){
            break;
        }
    }

    return true;
}



bool GroupInstMgr::getSegLong(long _longID, char * _sAreaCode,
    GroupInstMember * _aSegInst, int _iTotalCnt, char *_sDate,
    int iGroupType, vector<GroupInstMember *> &_vecGroupInst)
{

    int iHead = 0, iTail = _iTotalCnt-1;
    int iMid ;
    if (_iTotalCnt <= 0) return false;
    // 折半查找,
    while (iHead <= iTail) {
        iMid = (iHead + iTail)/2;
        int iRet = _longID - _aSegInst[iMid].m_lServID;
        if (iRet > 0) {
            iHead = iMid+1;
            continue;
        }
        if (iRet < 0) {
            iTail = iMid-1;
            continue;
        }
        //found:
        break;
    }

    //目标: 让iMid 指向 最后一个满足条件的记录.
    int iRet = _longID - _aSegInst[iMid].m_lServID;
    if (iRet < 0) {
        iMid--;
    }
    else if (iRet == 0) { //游标试图向后增大查找是否有与当前 m_lServID 相同的记录
        while (iMid<_iTotalCnt-1) {
            iMid++;
            if (_longID < _aSegInst[iMid].m_lServID){
                iMid--;
                break;
            }
        }
    }

    // 向前遍历匹配
    for ( ; iMid>=0 ; iMid--) {

        int iRetA = _longID - _aSegInst[iMid].m_lServID_2;
        if (iRetA <= 0) { //  在当前号段内
            if(_sAreaCode != NULL
              && strcmp(_sAreaCode, _aSegInst[iMid].m_sAreaCode)!=0 ){
                continue;
            }
            if( (iGroupType<=0 || _aSegInst[iMid].m_iGroupTypeID==iGroupType)
                && checkDate(_aSegInst[iMid].m_iLifeCycleID, _sDate) )
                _vecGroupInst.push_back( _aSegInst + iMid );

        }
        // 判断向前是否还有记录
        if (_longID >  _aSegInst[iMid].m_lObjectID){
            break;
        }
    }

    return true;
}


//////////////////////////////////////////////

////根据号码获取所有符合条件的群ID
////iGroupType=0的话,获取所有的type
bool GroupInstMgr::getGroupInst (
         char*   sAreaCode,
         char*   sAccNbr,
         char*   sDate,
         int     iGroupType,
         vector<GroupInstMember *> &o_vecGroupInst  )
{
    GroupInstMember * pInstMember=NULL;
    if(m_phpAccNbr!=NULL && m_phpAccNbr->get(sAccNbr, &pInstMember) ){
        while(pInstMember){
            if(strcmp(sAreaCode, pInstMember->m_sAreaCode)==0
                && (iGroupType<=0 || pInstMember->m_iGroupTypeID ==iGroupType)
                && checkDate(pInstMember->m_iLifeCycleID, sDate) ){
                o_vecGroupInst.push_back(pInstMember);
            }
            pInstMember = pInstMember->m_pNext;
        }
    }

    getSegStr( sAreaCode, sAccNbr, sDate, iGroupType, o_vecGroupInst);
    
    ////如果都是数字,才使用整数的判断
    int iDigit=0;
    while(iDigit<19){
        if(sAccNbr[iDigit]=='\0')
            break;
        if (sAccNbr[iDigit]<'0' || sAccNbr[iDigit]>'9' ){
            iDigit=20;
            break;
        }
        iDigit++;    
    }
    if(iDigit<19){
        getSegLong( atol(sAccNbr), sAreaCode, m_aSegAccNbrLong, m_iSegAccNbrLong,
            sDate, iGroupType, o_vecGroupInst);
    }

    if(m_phpNestGroup!=NULL){
        int ii=o_vecGroupInst.size();
        for(int iTemp=0; iTemp<ii; iTemp++){
            if(m_phpNestGroup->get(o_vecGroupInst[iTemp]->m_lGroupID,&pInstMember)){
                if(iGroupType<=0 || pInstMember->m_iGroupTypeID ==iGroupType){
                    o_vecGroupInst.push_back(pInstMember);
                    ii++;
                    if(ii>100) break;
                }
            }
        }
    }

    return true;
}

////根据servID获取所有符合条件的群ID
////iGroupType=0的话,获取所有的type
bool GroupInstMgr::getGroupInst (
         long    lServID,
         char*   sDate,
         int     iGroupType,
         vector<GroupInstMember *> &o_vecGroupInst )
{
    GroupInstMember * pInstMember=NULL;
    if(m_phpServID!=NULL && m_phpServID->get(lServID, &pInstMember) ){
        while(pInstMember){
            if(iGroupType<=0 || pInstMember->m_iGroupTypeID ==iGroupType){
                if(checkDate(pInstMember->m_iLifeCycleID, sDate) ){
                    o_vecGroupInst.push_back(pInstMember);
                }
            }
            pInstMember = pInstMember->m_pNext;
        }
    }

    ////servID查找的时候不去看 AreaCode
    getSegLong(lServID,NULL,m_aSegServID,m_iSegServID,sDate,
        iGroupType,o_vecGroupInst);


    if(m_phpNestGroup!=NULL){
        int ii=o_vecGroupInst.size();
        for(int iTemp=0; iTemp<ii; iTemp++){
            if(m_phpNestGroup->get(o_vecGroupInst[iTemp]->m_lGroupID,&pInstMember)){
                if(iGroupType<=0 || pInstMember->m_iGroupTypeID ==iGroupType){
                    o_vecGroupInst.push_back(pInstMember);
                    ii++;
                    if(ii>100) break;
                }
            }
        }
    }
    return true;
}


////判断一个号码是否在某个群
bool GroupInstMgr::ifBelong (
         char*   sAreaCode,
         char*   sAccNbr,
         char*   sDate,
         long    lGroupID )
{
    vector<GroupInstMember *> o_vecGroupInst;
    getGroupInst(sAreaCode, sAccNbr, sDate, 0, o_vecGroupInst);
    
    for(int iTmp=0; iTmp<o_vecGroupInst.size(); iTmp++){
        if(lGroupID == o_vecGroupInst[iTmp]->m_lGroupID)
            return true;
    }
    return false;
    
}



////判断一个servID是否在某个群
bool GroupInstMgr::ifBelong (
         long    lServID,
         char*   sDate,
         long    lGroupID )
{
    vector<GroupInstMember *> o_vecGroupInst;
    getGroupInst(lServID, sDate, 0, o_vecGroupInst);
    
    for(int iTmp=0; iTmp<o_vecGroupInst.size(); iTmp++){
        if(lGroupID == o_vecGroupInst[iTmp]->m_lGroupID)
            return true;
    }
    return false;
    
}


////判断一个号码是否存在一个属于某类型的群
bool GroupInstMgr::ifExist (
         char*   sAreaCode,
         char*   sAccNbr,
         char*   sDate,
         int     iGroupType )
{
    vector<GroupInstMember *> o_vecGroupInst;
    getGroupInst(sAreaCode, sAccNbr, sDate, iGroupType, o_vecGroupInst);
    
    if(o_vecGroupInst.size()>0)
        return true;
    return false;
    
}



////判断一个servID是否存在一个属于某类型的群
bool GroupInstMgr::ifExist (
         long    lServID,
         char*   sDate,
         int     iGroupType )
{
    vector<GroupInstMember *> o_vecGroupInst;
    getGroupInst(lServID, sDate, iGroupType, o_vecGroupInst);
    
    if(o_vecGroupInst.size()>0)
        return true;
    return false;
    
}



////两个servID是否在同一个群
bool GroupInstMgr::ifSameGroup (
         long    lServID,
         long    lServID2,
         char*   sDate,
         int     iGroupType)
{
    vector<GroupInstMember *> o_vecGroupInst;
    vector<GroupInstMember *> o_vecGroupInst_2;
    
    getGroupInst(lServID, sDate, iGroupType, o_vecGroupInst);
    getGroupInst(lServID2, sDate, iGroupType, o_vecGroupInst_2);
    
    for(int iTmp=0; iTmp<o_vecGroupInst.size(); iTmp++){
        if(iGroupType>0 && o_vecGroupInst[iTmp]->m_iGroupTypeID !=iGroupType)
            continue;
        for(int jTmp=0; jTmp<o_vecGroupInst_2.size(); jTmp++){
            if(o_vecGroupInst[iTmp]->m_lGroupID==o_vecGroupInst_2[jTmp]->m_lGroupID)
                return true;
        }
    }
    return false;
    
}


////号码和servID是否在同一个群
bool GroupInstMgr::ifSameGroup (
         long    lServID,
         char*   sAreaCode,
         char*   sAccNbr,
         char*   sDate,
         int     iGroupType )
{
    vector<GroupInstMember *> o_vecGroupInst;
    vector<GroupInstMember *> o_vecGroupInst_2;
    
    getGroupInst(lServID, sDate, iGroupType, o_vecGroupInst);
    getGroupInst(sAreaCode, sAccNbr, sDate, iGroupType, o_vecGroupInst_2);
    
    for(int iTmp=0; iTmp<o_vecGroupInst.size(); iTmp++){
        if(iGroupType>0 && o_vecGroupInst[iTmp]->m_iGroupTypeID !=iGroupType)
            continue;
        for(int jTmp=0; jTmp<o_vecGroupInst_2.size(); jTmp++){
            if(o_vecGroupInst[iTmp]->m_lGroupID==o_vecGroupInst_2[jTmp]->m_lGroupID)
                return true;
        }
    }
    return false;
}



////两个号码是否在同一类型的群
bool GroupInstMgr::ifSameGroup (
         char*   sAreaCode,
         char*   sAccNbr,
         char*   sAreaCode2,
         char*   sAccNbr2,
         char*   sDate,
         int     iGroupType )

{
    vector<GroupInstMember *> o_vecGroupInst;
    vector<GroupInstMember *> o_vecGroupInst_2;
    
    getGroupInst(sAreaCode, sAccNbr, sDate, iGroupType, o_vecGroupInst);
    getGroupInst(sAreaCode2, sAccNbr2, sDate, iGroupType, o_vecGroupInst_2);
    
    for(int iTmp=0; iTmp<o_vecGroupInst.size(); iTmp++){
        if(iGroupType>0 && o_vecGroupInst[iTmp]->m_iGroupTypeID !=iGroupType)
            continue;
        for(int jTmp=0; jTmp<o_vecGroupInst_2.size(); jTmp++){
            if(o_vecGroupInst[iTmp]->m_lGroupID==o_vecGroupInst_2[jTmp]->m_lGroupID)
                return true;
        }
    }
    return false;
    
}

