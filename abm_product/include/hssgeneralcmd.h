/*
*   version js at 2011-5-18
*/


#ifndef    __HSSCMDGENERAL_H_INCLUDED_
#define    __HSSCMDGENERAL_H_INCLUDED_

#include "hssobject.h"
#include "hssconstdef.h"
#include <vector>

using namespace std;


typedef long  __DFT_T1_;
typedef __DFT_T1_ __DFT_T2_;
typedef __DFT_T1_   __DFT_T3_;

#define UTF8_MAX 64
typedef unsigned int ui32;
typedef unsigned long ul64;

/**************************************************************************
*   类名：AccuTpl
*   功能：定义了累积量的命令体的模板类
*   说明：
*       1. __clear(): 数据结构体初始化
*       2. __add() :  封装累积量的数据
*       3. __getValue(): 获取本命令中的累积量数据
***************************************************************************/

template <typename T1,typename T2,  int CMD>
class GeneralTpl : public HSSObject
{
    
public:
    
    GeneralTpl() : HSSObject(CMD) {
    	__clear();
    };
    
    bool empty() {
    	return !(*((long *)&m_uBody));
    }
    
    int __getValue(T1 *&ptr) {
        int size = sizeof(m_uBody.m_tNum);  
        ptr = (T1 *)&m_uBody.m_sBuf[size]; 
        return m_uBody.m_tNum.m_iFirstNum;
    } 
    
    int __getValue2(T2 *&ptr) {
        int size = sizeof(m_uBody.m_tNum);  
        ptr = (T2 *)&m_uBody.m_sBuf[size]; 
        return m_uBody.m_tNum.m_iSecondNum;
    } 

    
protected:  
    
    void __clear() {
        m_iSize = CMD_OFFSET + sizeof(m_uBody.m_tNum);
        memset(&m_uBody.m_tNum, 0x00, sizeof(m_uBody.m_tNum));
    }
    
    bool __add1(T1 &data) {
        if((m_iSize+sizeof(T1)) > CMD_BUF_MAX) {
            return false;
        }
        m_uBody.m_tNum.m_iFirstNum ++;
        memcpy((void *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET], (const void *)&data, sizeof(T1));
        m_iSize += sizeof(T1);
        return true;
    }

    bool __add2(T2 &data) {
        if((m_iSize+sizeof(T2)) > CMD_BUF_MAX) {
            return false;
        }
        m_uBody.m_tNum.m_iSecondNum ++;
        memcpy((void *)&m_uBody.m_sBuf[m_iSize-CMD_OFFSET], (const void *)&data,sizeof(T2));
        m_iSize += sizeof(T2);
        return true;
    }
       
    
    int __getValue(vector<T1 *> &vec) {
    	int _size;
        for (int i=0; i<m_uBody.m_tNum.m_iFirstNum; ++i) {
        	_size = i*sizeof(T1) + sizeof(m_uBody.m_tNum);
        	if (_size >= CMD_BUF_SIZE)
        		return -1;
            vec.push_back((T1 *)&m_uBody.m_sBuf[_size]);  
        }   
        return vec.size();                  
    }
    
    int __getValue2(vector<T2 *> &vec) {
    	int _size;
        for (int i=0; i<m_uBody.m_tNum.m_iSecondNum; ++i) {
        	_size = i*sizeof(T2) + sizeof(m_uBody.m_tNum);
        	if (_size >= CMD_BUF_SIZE)
        		return -1;
            vec.push_back((T2 *)&m_uBody.m_sBuf[_size]);  
        }   
        return vec.size();
    } 
};

//7.2.5.1用户信息查询
//请求消息
struct QueryServCCR
{
    int  m_iValueType;           //查询值类型：1-业务号码；2-客户标识；3-合同号标识
    ui32 m_uiQueryType;          //查询类型（1：查询最新记录，2：查询所有记录）
    char m_sValue[UTF8_MAX];     //查询值
    char m_sAreaCode[UTF8_MAX];  //电话区号
};

class HssQueryServCCR : public GeneralTpl<QueryServCCR,__DFT_T2_,HSSCMD_SERV_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryServCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryServCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryServCCR *> &vData)
    {
        return __getValue(vData);
    }
};
//返回CCA
struct QueryServCCA
{
    int  m_iAlign;               //对齐
    ui32 m_uiServState;          //用户状态0：正常；1：停机；2：欠费停机；3：拆机
    ul64 m_ulServId;             //用户标识
    ui32 m_uiProductId;          //产品标识
    ui32 m_uiHomeAreaCode;       //本地网区号
    char m_sStateDate[UTF8_MAX]; //状态时间
    ui32 m_uiPaymentFlag;        //付费方式，0：后付费，1：预付费
    ui32 m_uiGroupUser;          //用户属性，0：拥有集团帐户，1：没有集团用户
    char m_sCustName[256];       //客户名称
    char m_sProductName[56];       //产品名称
};
class HssQueryServCCA : public GeneralTpl<QueryServCCA,__DFT_T2_,HSSCMD_SERV_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryServCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryServCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryServCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.2附属产品信息查询
//请求CCR 使用用户信息查询的请求结构QueryServCCR
class HssQueryServProductCCR : public GeneralTpl<QueryServCCR,__DFT_T2_,HSSCMD_SERVPRODUCT_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryServCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryServCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryServCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//返回CCA
struct QueryServProductCCA
{
    ul64 m_ulServProductID;    //附属产品实例标识
    char m_sServProductName[56]; //附属产品名称
    ul64 m_ulServId;           //归属主产品实例标识
    ui32 m_uiProductId;        //附属产品标识
    char m_sProductName[52];   //主产品名称
    ui32 m_uiEffDate; //生效时间
    ui32 m_uiExpDate; //失效时间
};
class HssQueryServProductCCA : public GeneralTpl<QueryServProductCCA,__DFT_T2_,HSSCMD_SERVPRODUCT_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryServProductCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryServProductCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryServProductCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.3账户信息查询
//CCR 使用用户信息查询的请求结构QueryServCCR
class HssQueryAcctCCR : public GeneralTpl<QueryServCCR,__DFT_T2_,HSSCMD_ACCT_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryServCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryServCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryServCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//返回CCA
struct QueryAcctCCA
{
    ul64 m_ulCustId;                     //客户标识
    char m_sCustName[UTF8_MAX];          //客户名称
    ul64 m_ulAcctId;                     //帐户标识
    char m_sTelAcctName[UTF8_MAX];       //账户名称
    char m_sAcctType[UTF8_MAX];          //帐户类型
    char m_sState[UTF8_MAX];             //状态
    char m_sStateDate[UTF8_MAX];         //状态时间
    ui32 m_uiPaymentMethod;              //付款方式
    int  m_iAlign;                       //字节对齐
};

class HssQueryAcctCCA : public GeneralTpl<QueryAcctCCA,__DFT_T2_,HSSCMD_ACCT_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryAcctCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryAcctCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryAcctCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.4客户信息查询
//CCR使用用户信息查询的请求CCR结构QueryServCCR
//QueryCustCCR
class HssQueryCustCCR : public GeneralTpl<QueryServCCR,__DFT_T2_,HSSCMD_CUST_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryServCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryServCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryServCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//返回CCA
struct QueryCustCCA
{
    char m_sCustName[UTF8_MAX];  //客户名称
    ui32 m_uiCustType;           //客户类型0：大客户；1：政企客户；2个人客户
    ui32 m_uiIdType;             //证件类型0：身份证；1：驾驶证；2：军官证；3护照；4：其他
    char m_sCustState[UTF8_MAX]; //客户状态
    char m_sEffDate[UTF8_MAX];   //生效时间
    char m_sExpDate[UTF8_MAX];   //失效时间
    char m_sIdCode[UTF8_MAX];    //证件编码
    ui32 m_uiHomeAreaCode;       //本地网区号
    int  m_iAlign;               //字节对齐
};

class HssQueryCustCCA : public GeneralTpl<QueryCustCCA,__DFT_T2_,HSSCMD_CUST_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryCustCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryCustCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryCustCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.18增值业务查询
//CCR使用用户信息查询的CCR结构QueryServCCR
class HssQueryIncrServCCR : public GeneralTpl<QueryServCCR,__DFT_T2_,HSSCMD_PRODUCTOFFER_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryServCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryServCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryServCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//返回CCA
struct QueryIncrServCCA
{
    ul64 m_ulUnitPrice;   //单价
    char m_sUnit[UTF8_MAX];            //单位
    char m_sSpDescription[UTF8_MAX];   //Sp描述（中文）
    char m_sIncrServName[UTF8_MAX];    //增值业务名称
    ui32 m_uiEffDate;         //生效时间
    ui32 m_uiExpDate;         //失效时间
    ui32 m_uiCreatedDate;     //订购日期
    int  m_iAlign;            //字节对齐
};

class HssQueryIncrServCCA : public GeneralTpl<QueryIncrServCCA,__DFT_T2_,HSSCMD_PRODUCTOFFER_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryIncrServCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryIncrServCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryIncrServCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.5套餐订购查询
struct OrderResourceCCR
{
    char m_sAccNbr[UTF8_MAX];
};
class HssOrderResourceCCR : public 
GeneralTpl<OrderResourceCCR,__DFT_T2_,HSSCMD_ORDERRESOURCE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(OrderResourceCCR &data)
    {
        return __add1(data);
    }

    int  getReq(OrderResourceCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<OrderResourceCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//返回CCA
struct OrderResourceCCA
{
    ui32 m_uiProductFlag;     //是否为累积量套餐（0：非累积量套餐1：累积量套餐）
    int  m_iAlign;                     //字节对齐
    char m_sDescription[256];     //套餐描述信息
    char m_sProductOFFName[UTF8_MAX];       //套餐名称
    ui32 m_uiEffDate;         //生效时间
    ui32 m_uiExpDate;         //失效时间
};
class HssOrderResourceCCA : public GeneralTpl<OrderResourceCCA,__DFT_T2_,HSSCMD_ORDERRESOURCE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(OrderResourceCCA &data)
    {
        return __add1(data);
    }

    int  getReq(OrderResourceCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<OrderResourceCCA *> &vData)
    {
        return __getValue(vData);
    }
};


//7.2.5.6.1通过群实例查询群成员
struct QueryGroupMemberCCR
{
    ul64 m_ulGroupId;   //群标识
    ui32 m_uiQueryType; //查询类型（1：查询最新记录；2：查询所有记录）
    int  m_iAlign;      //字节对齐
};
class HssQueryGroupMemberCCR : public GeneralTpl<QueryGroupMemberCCR,__DFT_T2_,HSSCMD_GROUPMEMBER_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryGroupMemberCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryGroupMemberCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryGroupMemberCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//返回CCA
struct QueryGroupMemberCCA
{
    char m_sMemberDesc[UTF8_MAX];      //群组实例成员描述
    ui32 m_uiMemberTypeId;             //群组实例成员类型表示
    ui32 m_uiMemberRoleId;             //群组实例成员角色标识
    ul64 m_ulServId;                   //主产品实例标识
    ui32 m_uiEffDate;         //生效时间
    ui32 m_uiExpDate;         //失效时间
    ul64 m_ulMemberObjectId;           //群组实例成员对象标识
    char m_sAccNbr[UTF8_MAX];          //成员号码
};
class HssQueryGroupMemberCCA : public GeneralTpl<QueryGroupMemberCCA,__DFT_T2_,HSSCMD_GROUPMEMBER_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryGroupMemberCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryGroupMemberCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryGroupMemberCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.6.2通过群成员查找群信息
//CCR
struct QueryGroupCCR
{
    char m_sAccNbr[UTF8_MAX];     //成员号码
    ui32 m_uiQueryType;           //查询类型（1：查询最新记录；2：查询所有记录）
    int  m_iAlign;                //对齐
};

class HssQueryGroupCCR : public GeneralTpl<QueryGroupCCR,__DFT_T2_,HSSCMD_GROUP_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryGroupCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryGroupCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryGroupCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//返回CCA
struct QueryGroupCCA
{
    ul64 m_ulGroupId;           //群组实例标识
    ui32 m_uiGroupTypeId;       //群组类型标识
    int  m_iAlign;
    char m_sGroupDesc[UTF8_MAX];//群组实例描述
};
class HssQueryGroupCCA : public GeneralTpl<QueryGroupCCA,__DFT_T2_,HSSCMD_GROUP_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryGroupCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryGroupCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryGroupCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.7特殊号头查询
//CCR
struct IsExistHeadCCR
{
    char m_sHeadType[UTF8_MAX];  //号头类型
    char m_sAccNbr[UTF8_MAX];    //电话号码
    ui32 m_uiHomeAreaCode;       //本地网区号
    int  m_iAlign;               //字节对齐
};
class HssIsExistHeadCCR : public GeneralTpl<IsExistHeadCCR,__DFT_T2_,HSSCMD_EXISTHEAD_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(IsExistHeadCCR &data)
    {
        return __add1(data);
    }

    int  getReq(IsExistHeadCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<IsExistHeadCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct IsExistHeadCCA
{
    ui32 m_iQueryResult;   //查询结果
    int  m_iAlign;         //字节对齐
};
class HssIsExistHeadCCA : public GeneralTpl<IsExistHeadCCA,__DFT_T2_,HSSCMD_EXISTHEAD_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(IsExistHeadCCA &data)
    {
        return __add1(data);
    }

    int  getReq(IsExistHeadCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<IsExistHeadCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.8归属查询（按号码）
//CCR
struct QueryHomeAreaByAccNbrCCR
{
    char m_sAccNbr[UTF8_MAX];  //电话号码
};
class HssQueryHomeAreaByAccNbrCCR : public 
GeneralTpl<QueryHomeAreaByAccNbrCCR,__DFT_T2_,HSSCMD_HOMEBYACCNBR_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryHomeAreaByAccNbrCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryHomeAreaByAccNbrCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryHomeAreaByAccNbrCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct QueryHomeAreaByAccNbrCCA
{
    ui32 m_uiHomeAreaCode;           //用户归属地区号
    int  m_iAlign;                   //字节对齐
    char m_sProvincecode[UTF8_MAX];  //归属省区号
};
class HssQueryHomeAreaByAccNbrCCA : public 
GeneralTpl<QueryHomeAreaByAccNbrCCA,__DFT_T2_,HSSCMD_HOMEBYACCNBR_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryHomeAreaByAccNbrCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryHomeAreaByAccNbrCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryHomeAreaByAccNbrCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.9归属查询（按IMSI）
//CCR
struct QueryHomeAreaByIMSICCR
{
    char m_sIMSI[UTF8_MAX];    //IMSI号
};
class HssQueryHomeAreaByIMSICCR : public 
GeneralTpl<QueryHomeAreaByIMSICCR,__DFT_T2_,HSSCMD_HOMEBYIMSI_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryHomeAreaByIMSICCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryHomeAreaByIMSICCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryHomeAreaByIMSICCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct QueryHomeAreaByIMSICCA
{
    ui32 m_uiHomeAreaCode;           //用户归属地区号
    int  m_iAlign;                   //字节对齐
    char m_sProvince_code[UTF8_MAX]; //归属省区号
};

class HssQueryHomeAreaByIMSICCA : public 
GeneralTpl<QueryHomeAreaByIMSICCA,__DFT_T2_,HSSCMD_HOMEBYIMSI_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryHomeAreaByIMSICCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryHomeAreaByIMSICCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryHomeAreaByIMSICCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.10H码查询
//CCR
struct QueryHCodeCCR
{
    char m_sAccNbr[UTF8_MAX];      //电话号码
    ui32 m_uiQueryType;            //查询方式（1:查询最新记录；2，查询全部记录）
    int  m_iAlign;                 //字节对齐
};

class HssQueryHCodeCCR : public GeneralTpl<QueryHCodeCCR,__DFT_T2_,HSSCMD_HCODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryHCodeCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryHCodeCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryHCodeCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct QueryHCodeCCA
{
    char m_sHead[UTF8_MAX];       //号段
    ui32 m_uiEmulatoryPartnerId;  //运营商标识
    ui32 m_uiHomeAreaCode;        //本地网区号
    char m_sEffDate[UTF8_MAX];    //生效时间
    char m_sExpDate[UTF8_MAX];    //失效时间
};

class HssQueryHCodeCCA : public GeneralTpl<QueryHCodeCCA,__DFT_T2_,HSSCMD_HCODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryHCodeCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryHCodeCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryHCodeCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.11本地号头表查询
//CCR
struct QueryLocalHeadCCR
{
    char m_sAccNbr[UTF8_MAX];      //电话号码
    ui32 m_uiQueryType;            //查询方式（1:查询最新记录；2，查询全部记录）
    int  m_iAlign;                 //字节对齐
};
class HssQueryLocalHeadCCR : public GeneralTpl<QueryLocalHeadCCR,__DFT_T2_,HSSCMD_LOCALHEAD_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryLocalHeadCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryLocalHeadCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryLocalHeadCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct QueryLocalHeadCCA
{
    ui32 m_uiHeadId;                  //号头标识
    char m_sHead[UTF8_MAX];           //号头
    ui32 m_uiEmulatoryPartnerId;      //运营商标识
    ui32 m_uiExchangeId;              //营业区标识
    char m_sEffDate[UTF8_MAX];        //生效时间
    char m_sExpDate[UTF8_MAX];        //失效时间
};
class HssQueryLocalHeadCCA : public GeneralTpl<QueryLocalHeadCCA,__DFT_T2_,HSSCMD_LOCALHEAD_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryLocalHeadCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryLocalHeadCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryLocalHeadCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.12边漫判断
//CCR
struct QueryBorderRoamingCCR
{
    ui32 m_uiBorderZone;            //临接本地网区号
    char m_sMscID[UTF8_MAX];        //交换机标识
    char m_sCellID[UTF8_MAX];       //蜂窝标识
    char m_sStartTime[UTF8_MAX];    //时间
};

class HssQueryBorderRoamingCCR : public 
GeneralTpl<QueryBorderRoamingCCR,__DFT_T2_,HSSCMD_BORDERROAM_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryBorderRoamingCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryBorderRoamingCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryBorderRoamingCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct QueryBorderRoamingCCA
{
    ui32 m_uiQueryResult;    //查询结果
    int  m_iAlign;           //字节对齐
};
class HssQueryBorderRoamingCCA : public 
GeneralTpl<QueryBorderRoamingCCA,__DFT_T2_,HSSCMD_BORDERROAM_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryBorderRoamingCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryBorderRoamingCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryBorderRoamingCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.13C网国际漫游运营商
//CCR
struct QueryIbrstCodeCCR
{
    ui32 m_uiQuerytype;              //2 查所有 1查指定运营商信息
    int  m_iAlign;                   //字节对齐
    char m_sCarrierCode[UTF8_MAX];   //被访运营商代码
};
class HssQueryIbrstCodeCCR : public GeneralTpl<QueryIbrstCodeCCR,__DFT_T2_,HSSCMD_IBRSTCODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryIbrstCodeCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryIbrstCodeCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryIbrstCodeCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct QueryIbrstCodeCCA
{
    char m_sCarrierName[UTF8_MAX];         //拜访运营商名称（英文）
    char m_sCtry[UTF8_MAX];                //拜访运营商所在国家代码
    ui32 m_uiSectorID;                     //所属资费区ID
    int  m_iAlign;                         //字节对齐
    char m_sEffDate[UTF8_MAX];             //生效日期
    char m_sExpDate[UTF8_MAX];             //失效时间
};
class HssQueryIbrstCodeCCA : public GeneralTpl<QueryIbrstCodeCCA,__DFT_T2_,HSSCMD_IBRSTCODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryIbrstCodeCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryIbrstCodeCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryIbrstCodeCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.14不同制式IMSI码查询
//CCR
struct QueryImsiCodeCCR
{
    char m_sIMSI[UTF8_MAX];    //IMSI码
};
class HssQueryImsiCodeCCR : public GeneralTpl<QueryImsiCodeCCR,__DFT_T2_,HSSCMD_IMSICODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryImsiCodeCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryImsiCodeCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryImsiCodeCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct QueryImsiCodeCCA
{
    ui32 m_uiSponsorCode;             //代理运营商
    ui32 m_uiHomeAreaCode;            //本地网区号
    char m_sEffDate[UTF8_MAX];        //生效日期
    char m_sExpDate[UTF8_MAX];        //失效日期
    ui32 m_uiImsiType;                //IMSI 类型0：普通号段；1：一卡三号
    int  m_iAlign;                    //字节对齐
};

class HssQueryImsiCodeCCA : public GeneralTpl<QueryImsiCodeCCA,__DFT_T2_,HSSCMD_IMSICODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryImsiCodeCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryImsiCodeCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryImsiCodeCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.15MIN码查询
//CCR
struct QueryMinCodeCCR
{
    char m_sMinNbr[UTF8_MAX];         //MIN码
};

class HssQueryMinCodeCCR : public GeneralTpl<QueryMinCodeCCR,__DFT_T2_,HSSCMD_MINCODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryMinCodeCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryMinCodeCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryMinCodeCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct QueryMinCodeCCA
{
    ui32 m_uiHomeAreaCode;          //本地网区号
    char m_sEffDate[UTF8_MAX];      //生效日期
    char m_sExpDate[UTF8_MAX];      //失效日期
    ui32 m_uiUserFlag;              //用户标识 1 国内；2国际。
    ui32 m_uiUserType;              //号段标识 2:189号段1:133和153号段0:国外运营商
    char m_sHomeCountryCode[UTF8_MAX]; //归属国家代码
    char m_sHomeCarrierCode[UTF8_MAX]; //归属C网运营商代码
};

class HssQueryMinCodeCCA : public GeneralTpl<QueryMinCodeCCA,__DFT_T2_,HSSCMD_MINCODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryMinCodeCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryMinCodeCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryMinCodeCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.16携号转网判断
//CCR
struct QueryNPCodeCCR
{
    char m_sAccNbr[UTF8_MAX];    //电话号码
    ui32 m_uiQueryType;          //查询类型（1：查询最新记录；2：查询所有记录）
    int  m_iAlign;               //字节对齐
};
class HssQueryNPCodeCCR : public GeneralTpl<QueryNPCodeCCR,__DFT_T2_,HSSCMD_NPCODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryNPCodeCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QueryNPCodeCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryNPCodeCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct QueryNPCodeCCA
{
    char m_sInNetID[UTF8_MAX];   //网络ID（携入方）
    char m_sOutNetID[UTF8_MAX];  //网络ID（携出方）
    char m_sOwnNetID[UTF8_MAX];  //网络ID（号码拥有网络）
    char m_sEffDate[UTF8_MAX];   //NP 启用时间
};
class HssQueryNPCodeCCA : public GeneralTpl<QueryNPCodeCCA,__DFT_T2_,HSSCMD_NPCODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QueryNPCodeCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QueryNPCodeCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QueryNPCodeCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.5.17SP判断
//CCR
struct QuerySPCodeCCR
{
    char m_sSpCode[UTF8_MAX];    //SP标识
    ui32 m_uiQueryType;          //查询类型（1：查询最新记录；2：查询所有记录）
    int  m_iAlign;               //字节对齐
};

class HssQuerySPCodeCCR : public GeneralTpl<QuerySPCodeCCR,__DFT_T2_,HSSCMD_SPCODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QuerySPCodeCCR &data)
    {
        return __add1(data);
    }

    int  getReq(QuerySPCodeCCR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QuerySPCodeCCR *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct QuerySPCodeCCA
{
    char m_sSpDescription[UTF8_MAX];  //Sp描述（中文）
    ui32 m_uiProvinceID;              //SP归属省标识
    ui32 m_uiSpFlag;                  //SP 标识 0：固网灵通短信sp4：移动业务的sp5：移动业务集团客户的sp
    char m_sEffDate[UTF8_MAX];        //生效日期
    char m_sExpDate[UTF8_MAX];        //失效日期
};
class HssQuerySPCodeCCA : public GeneralTpl<QuerySPCodeCCA,__DFT_T2_,HSSCMD_SPCODE_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(QuerySPCodeCCA &data)
    {
        return __add1(data);
    }

    int  getReq(QuerySPCodeCCA *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<QuerySPCodeCCA *> &vData)
    {
        return __getValue(vData);
    }
};

//7.2.4.1.3业务网元提供汇总信息查询接口
//CCR
struct stService
{
    char m_sQueryTypeID[8]; //查询类型标识
    char m_sQueryTime[24];  //静态查询截止时间
};

class HssDataReportCCR : public GeneralTpl<stService,__DFT_T2_,HSSCMD_DATAREPORT_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(stService &data)
    {
        return __add1(data);
    }

    int  getReq(stService *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<stService *> &vData)
    {
        return __getValue(vData);
    }
};

//CCA
struct stRltSrv
{
    unsigned int m_uiSrvRltCode;
    char m_sParaFieldRlt[132];
    unsigned int m_uiCustInfo;
    unsigned int m_uiAcctInfo;
    unsigned int m_uiServInfo;
    unsigned int m_uiProcInfo;
};

class HssDataReportCCA : public GeneralTpl<stRltSrv,__DFT_T2_,HSSCMD_DATAREPORT_QUERY>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(stRltSrv &data)
    {
        return __add1(data);
    }

    int  getReq(stRltSrv *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<stRltSrv *> &vData)
    {
        return __getValue(vData);
    }
};

struct struInfoApproveR
{
    char sOfferId[16];
    char sOfferCode[16];
    char sState[8];
    char sApproveInfo[128];
};

struct struFaildMsg
{
    char sMsg[128];
};

class InfoApproveCCR : public GeneralTpl<struInfoApproveR, __DFT_T2_, HSSCMD_OFFERDOMAIN_APPROVE>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(struInfoApproveR &data)
    {
        return __add1(data);
    }

    int  getReq(struInfoApproveR *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<struInfoApproveR *> &vData)
    {
        return __getValue(vData);
    }
};

class InfoApproveCCA : public GeneralTpl<struFaildMsg, __DFT_T2_, HSSCMD_OFFERDOMAIN_APPROVE>
{
public:
    void clear()
    {
        __clear();
    }
    bool addReq(struFaildMsg &data)
    {
        return __add1(data);
    }

    int  getReq(struFaildMsg *&data)
    {
        return __getValue(data);
    }

    int  getReq(vector<struFaildMsg *> &vData)
    {
        return __getValue(vData);
    }
};




#endif/*__HSSCMDGENERAL_H_INCLUDED_*/
