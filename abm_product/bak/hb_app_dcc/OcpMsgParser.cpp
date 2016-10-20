#include <string>
#include <sstream>
#include <iostream>
#include "OcpMsgParser.h"
#include "comlist.h"
#include "resultcodes.h"
#include "diameter_parser_api.h"
#include "Log.h"

#define GET_DATA_REF(dataType, data, containerEntryPtr) \
dataType &data = (containerEntryPtr)->dataRef(Type2Type<dataType>())

void* OcpMsgParser::getshm()
{
return m_shmServiceType;
}


void * OcpMsgParser::getShmAddr(key_t shmKey, int shmSize)
{
    int shmid;
    if((shmid=shmget(shmKey, shmSize, SHM_R|SHM_W|IPC_EXCL|IPC_CREAT))==-1)
    {
        if(errno==EEXIST)
            shmid=shmget(shmKey,0,0);
        else
            return NULL;
    }
    return shmat(shmid, NULL, 0);
}


/*
static char *request_scenario[4] =
{
    "I", "U", "F", "E"
};
*/

void print(const AAAAvpContainer & c)
{
    for(int j = 0; j < c.size(); j++)
    {
        AAAAvpContainerEntry * ce = c[j];
        if(ce->dataType() != AAA_AVP_GROUPED_TYPE)
        {
            cout << "\t\t数据: ";
        }
        else
        {
            cout << "\t\tNO." << j << endl;
        }
        switch(ce->dataType())
        {
        case AAA_AVP_INTEGER32_TYPE:
        case AAA_AVP_ENUM_TYPE:
        {
            cout << *ce->dataPtr(Type2Type<int>());
            break;
        }
        case AAA_AVP_STRING_TYPE:
        case AAA_AVP_UTF8_STRING_TYPE:
        case AAA_AVP_DIAMID_TYPE:
        {
            cout << ce->dataPtr(Type2Type<string>())->c_str();
            break;
        }
        case AAA_AVP_UINTEGER32_TYPE:
        {
            cout << *ce->dataPtr(Type2Type<unsigned int>());
            break;
        }
        case AAA_AVP_ADDRESS_TYPE:
        {
            diameter_address_t * addr = ce->dataPtr(Type2Type<diameter_address_t>());
            cout << (int)(unsigned char)addr->value.c_str()[0] << "."
                 << (int)(unsigned char)addr->value.c_str()[1] << "."
                 << (int)(unsigned char)addr->value.c_str()[2] << "."
                 << (int)(unsigned char)addr->value.c_str()[3]  <<endl;

            break;
        }
        case AAA_AVP_GROUPED_TYPE:
        {
            AAAAvpContainerList * acl = ce->dataPtr(Type2Type <AAAAvpContainerList>());
            AAAAvpContainerList::iterator iter;
            int i = 0;
            for(iter = acl->begin(); iter != acl->end(); iter++)
            {
                AAAAvpContainer * ccc = *iter;
                cout << ccc->getAvpName() << endl;
                print(*ccc);
            }
            break;
        }
        default:
        {
            cout << "no print";
            break;
        }
        }
        cout << endl;
    }

}


void OcpMsgParser::getServiceType(std::string &serviceContextId, char *serviceType)
{

    //std::string scenario;
    std::string::size_type pos1, pos2;
    std::string tmp;
    //strcpy(serviceType, "UNKNOWN");
    //pos1 = serviceContextId.find(".", 0);
    pos2 = serviceContextId.find("@", 0);
    /*
    if (pos1 != std::string::npos && pos2 != std::string::npos)
    {
        if (pos2 > pos1)
        {
            tmp = serviceContextId.substr(0, pos1);
        }
        else
        {
            tmp = serviceContextId.substr(0, pos2);
        }
        //只取@前面的
        tmp = serviceContextId.substr(0,pos2);

        strcpy(serviceType, tmp.c_str());
    }
    */
    //只取@前面的
    tmp = serviceContextId.substr(0,pos2);
    strcpy(serviceType, tmp.c_str());
}

void OcpMsgParser::setDictHandle(bool output)
{
    //modified by zhufengsheng 2011.03.17 非CCR/CCA类型的直接绑定默认字典
    if(m_oMsg->hdr.code  != COMMAND_CODE_CC)
    {
        ::setDictHandle(*m_oMsg, "", "", "");
        return ;
    }
    //end of modified by zhufengsheng 2011.03.17

    //以下处理CCR/CCA类型

    //CCR
    if(m_oMsg->hdr.code == COMMAND_CODE_CC && m_oMsg->hdr.flags.r == AAA_FLG_SET)
    {
        std::string serviceContextId;
        char serviceType[80];
        getAvpValue("Service-Context-Id", serviceContextId);
        //去掉@后面的
        getServiceType(serviceContextId, serviceType);
        //绑定到字典
        ::setDictHandle(*m_oMsg, "", serviceType, "");
        return ;
    }
    //CCA
    else  if(m_oMsg->hdr.code == COMMAND_CODE_CC && m_oMsg->hdr.flags.r == AAA_FLG_CLR)
    {
        //这里区分激活还是新业务的还是静态数据类的
        
        //std::string serviceContextId;
        char serviceType[80]={0};
		
        std::string sessionid ;
        getAvpValue("Session-Id", sessionid);
        char *p = strrchr((char*)sessionid.c_str(), ';');
        unsigned int hh = atol(p+1);
        memset(serviceType,0x00,sizeof(serviceType));
        strcpy(serviceType, m_shmServiceType+(hh%SHM_ITEM_NUM)*SHM_ITEM_LEN);
        //在共享内存中找到的话表示是新业务
        if(strcmp(serviceType,"") != 0)
        {
            //绑定到字典
            ::setDictHandle(*m_oMsg, "", serviceType, "");
			/*
	   if(strcmp(serviceType,"Active.Net-Ctrl")==0)
	   	{
	   	    memset(m_shmServiceType+(hh%SHM_ITEM_NUM)*SHM_ITEM_LEN,0x00,SHM_ITEM_LEN);
	   	}*/
            return ;
        }
	//没有找到表示是静态数据类的业务
        else
        {
            //绑定到静态数据类的字段NP.Para
            ::setDictHandle(*m_oMsg, "", "NP.Para", "");
            return ;
        }
    }
    cout<<"错误的消息类型"<<endl;
}

int OcpMsgParser::getServiceInfo(char *buf, std::string &serviceContextId, std::string &serviceIdentifier)
{
    char tmp_buf[80];
    serviceContextId = std::string("");
    serviceIdentifier = std::string("");
    if (strncmp(buf, "55AA", 4) == 0)
    {
        memcpy(tmp_buf, &buf[4], 4);
        tmp_buf[4] = 0;
        int len = atoi(tmp_buf);
        memcpy(tmp_buf, &buf[8], len - 8);
        char *p1,  *p2, tmp_buf1[80];
        if (tmp_buf[0] == '|')
        {
            if ((p1 = (char*)strstr(&tmp_buf[1], "|")) != NULL)
            {
                memset(tmp_buf1, 0, 80);
                memcpy(tmp_buf1, &tmp_buf[1], p1 - tmp_buf - 1);
                serviceContextId = std::string(tmp_buf1);
                if ((p2 = (char*)strstr(p1 + 1, "|")) != NULL)
                {
                    memset(tmp_buf1, 0, 80);
                    memcpy(tmp_buf1, p1 + 1, p2 - p1 - 1);
                    serviceIdentifier = std::string(tmp_buf1);
                }
            }
        }
        return len;
    }
    else
        return 0;
}

int OcpMsgParser::parseRawToApp(void *buf, int bufSize, ParseOption option)
{
    AAAMessageBlock *aBuffer;
    HeaderParser hp;

    m_oMsg->acl.releaseContainers();
    setSetFlag(false);
    aBuffer = AAAMessageBlock::Acquire((char*)buf, bufSize);

    hp.setRawData(aBuffer);
    hp.setAppData(&m_oMsg->hdr);
    hp.setDictData(option);
    try
    {
        hp.parseRawToApp();
    }
    catch (AAAErrorStatus st)
    {
        std::cout << "header error" << std::endl;
        int type, code;
        std::string avp;
        st.get(type, code, avp);
        aBuffer->Release();
        return code;
    }

    ::setRequestFlag(m_oMsg->hdr.flags.r);
    //add by zhufengsheng 2011.03.17
    ::setDictHandle(*m_oMsg, "", "", "");
    //end of add by zhufengsheng 2011.03.17
    PayloadParser pp;
    aBuffer->size(m_oMsg->hdr.length);
    pp.setRawData(aBuffer);
    pp.setAppData(&m_oMsg->acl);
    pp.setDictData(m_oMsg->hdr.getDictHandle());
    if (m_oMsg->hdr.code == COMMAND_CODE_CC && option == PARSE_STRICT)
        pp.setListNum(1);
    else
        pp.setListNum(4);

    if (m_oMsg->hdr.code == COMMAND_CODE_CC && m_oMsg->hdr.flags.r == AAA_FLG_CLR)
    	{
    	        pp.setListNum(4);
    	}

    try
    {
        pp.parseRawToApp();
    }
    catch (AAAErrorStatus st)
    {
        std::cout << "1.payload parser error" << std::endl;
        int type, code;
        std::string avp;
        st.get(type, code, avp);

#ifdef DEBUG
        std::cout << "Error type=" << type << ", code="
                  << code << ", name=" << avp.c_str() << std::endl;
#endif
        //     		m_oMsg->acl.releaseContainers();

        aBuffer->Release();

        return code;
    }

    //	printf("2.msg.hdr.length=%d\n", m_oMsg->hdr.length);

    if (m_oMsg->hdr.code == COMMAND_CODE_CC && option == PARSE_STRICT)
    {
        //绑定到具体业务场景的字典
        this->setDictHandle();
        aBuffer->rd_ptr(aBuffer->base() + HEADER_SIZE);
        aBuffer->size(m_oMsg->hdr.length);
        pp.setRawData(aBuffer);
        pp.setAppData(&m_oMsg->acl);

        pp.setDictData(m_oMsg->hdr.getDictHandle());
        pp.setListNum(3);
        try
        {
            pp.parseRawToApp();
        }
        catch (AAAErrorStatus st)
        {
            std::cout << "2.payload parser error" << std::endl;
            int type, code;
            std::string avp;
            st.get(type, code, avp);
            m_sFailedAvp = avp;
#ifdef DEBUG

            std::cout << "Error type=" << type << ", code="
                      << code << ", name=" << avp.c_str() << std::endl;
#endif

            //    		m_oMsg->acl.releaseContainers();
            aBuffer->Release();
            return  code;
        }

    }
    aBuffer->Release();
    // printf("parseRawToApp ok\n");
    return 0;
}

int OcpMsgParser::parseRawToApp(std::string &str_buf, ParseOption option)
{
    return parseRawToApp((unsigned char *) str_buf.data(), str_buf.length(), option);
}



void OcpMsgParser::printHeader()
{
    AAADiameterHeader &h = m_oMsg->hdr;
    std::cout << "version = " << (int)h.ver << std::endl;
    std::cout << "length=" << h.length << "\n";
    std::cout << "code = " << h.code << std::endl;
    std::cout << "flags(r,p,e,t)=("
              << (int)h.flags.r << ","
              << (int)h.flags.p << ","
              << (int)h.flags.e << ","
              << (int)h.flags.t << ")" << std::endl;
    std::cout << "applicationId = " << h.appId << std::endl;
    std::cout << "h-h id = " << h.hh << std::endl;
    std::cout << "e-e id = " << h.ee << std::endl;

    //Log::log(0,"version =%d",(int)h.ver);
    //Log::log(0,"length = %d",h.length);
    //Log::log(0,"code = %d",h.code);
    //Log::log(0,"flags(r,p,e,t) = (%d,%d,%d,%d)",(int)h.flags.r,(int)h.flags.p,(int)h.flags.e,(int)h.flags.t);
    //Log::log(0,"applicationId = %d",h.appId);
    //Log::log(0,"h-h id = %d",h.hh);
    //Log::log(0,"e-e id = %d",h.ee);
    // add by zhangch
    char temp[256] = {0};
    sprintf(temp, "version =%d\n", (int)h.ver);
    m_sAvpOutStr+=temp;
    memset(temp, 0, 255);
    sprintf(temp, "length = %d\n", h.length);
    m_sAvpOutStr+=temp;
    memset(temp, 0, 255);
    sprintf(temp, "code = %d\n", h.code);
    m_sAvpOutStr+=temp;
    memset(temp, 0, 255);
    sprintf(temp, "flags(r,p,e,t) = (%d,%d,%d,%d)\n", (int)h.flags.r,(int)h.flags.p,(int)h.flags.e,(int)h.flags.t);
    m_sAvpOutStr+=temp;
    memset(temp, 0, 255);
    sprintf(temp, "applicationId = %d\n", h.appId);
    m_sAvpOutStr+=temp;
    memset(temp, 0, 255);
    sprintf(temp, "h-h id = %d\n", h.hh);
    m_sAvpOutStr+=temp;
    memset(temp, 0, 255);
    sprintf(temp, "e-e id = %d\n", h.ee);
    m_sAvpOutStr+=temp;
}

int OcpMsgParser::parseAppToRaw( void *buf, int size, bool output, bool bad_answer, ParseOption option  )
{
    AAAMessageBlock *aBuffer;
    HeaderParser hp;
    setSetFlag(false);
    aBuffer = AAAMessageBlock::Acquire((char*)buf, size);
    hp.setRawData(aBuffer);
    hp.setAppData(&m_oMsg->hdr);
    hp.setDictData(option);

    /* Check validity of flags and gets a AAACommand structure */
    try
    {
        hp.parseAppToRaw();
    }
    catch (AAAErrorStatus st)
    {
#ifdef DEBUG
        std::cout << "header error" << std::endl;
#endif
        aBuffer->Release();
        return  - 1;
    }

    ACE_INT32 int32 = AAA_SUCCESS;
    getAvpValue("Result-Code", int32);
    ::setResultCode(m_oMsg->hdr.flags.r, int32);
    
    if (m_oMsg->hdr.code == COMMAND_CODE_CC && option == PARSE_STRICT)
    {
        if (bad_answer)
        {
            ::setDictHandle(*m_oMsg, "", "", "ERROR");
        }
        else
            this->setDictHandle(output);
    }

    if (output)
    {
        if( (m_oMsg->hdr.code == COMMAND_CODE_RA || m_oMsg->hdr.code == COMMAND_CODE_AS ) && option == PARSE_STRICT )
        {
            ::setDictHandle(*m_oMsg, "", "", "OUT");
        }
    }
    
    //绑定到字典modified by zhufengsheng 2011.03.17
    //this->setDictHandle(output);
    //绑定到字典end of modified by zhufengsheng 2011.03.17
    
    PayloadParser pp;
    pp.setRawData(aBuffer);
    pp.setAppData(&m_oMsg->acl);
    pp.setDictData(m_oMsg->hdr.getDictHandle());
    //		pp.setListNum(4);

    try
    {
        pp.parseAppToRaw();
    }
    catch (AAAErrorStatus st)
    {
#ifdef DEBUG
        std::cout << "assemble failed" << std::endl;
#endif
        getAllAvp();
        m_oMsg->acl.releaseContainers();
        aBuffer->Release();
        return  - 1;
    }

    m_oMsg->hdr.length = aBuffer->wr_ptr() - aBuffer->base();

    /* The second calll of hp.set() sets the actual message length */
    try
    {
        hp.parseAppToRaw();
    }
    catch (AAAErrorStatus st)
    {
#ifdef DEBUG
        std::cout << "header error" << std::endl;
#endif
        m_oMsg->acl.releaseContainers();
        aBuffer->Release();
        return  - 1;
    }

    //m_oMsg->acl.releaseContainers();
    aBuffer->Release();
    return (m_oMsg->hdr.length);
};

int OcpMsgParser::parseAppToRaw( std::string& str_buf, bool output, ParseOption option)
{
    unsigned char buf[8192];
    int len;
    if ((len = parseAppToRaw(buf, 8192, output, false, option)) > 0)
    {
        str_buf.assign((char*)buf, len);
        return len;
    }
    return  - 1;

}

int OcpMsgParser::deleteAvpValue(char *avp_name1)
{
    AAAAvpContainer *c;
    int quit_flag = 0;
    std::string::size_type pos, start_pos;
    std::string avpName, avp_name = std::string(avp_name1);
    std::string::size_type pos1, pos2, pos3;
    AAAAvpContainerList *acl = &m_oMsg->acl;
    for (pos = 0, start_pos = 0; !quit_flag;)
    {
        pos1 = avp_name.find(".", start_pos);
        if (pos1 != std::string::npos)
        {
            avpName = avp_name.substr(start_pos, pos1 - start_pos);
            pos2 = avpName.find("[", 0);
            if (pos2 != std::string::npos)
            {
                pos3 = avpName.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avpName.substr(pos2 + 1, pos3 - pos2 - 1).c_str());
                avpName.erase(pos2, 10);
            }
            start_pos = pos1 + 1;
        }
        else
        {
            pos2 = avp_name.find("[", start_pos);
            if (pos2 != std::string::npos)
            {
                pos3 = avp_name.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avp_name.substr(pos2 + 1, pos3 - pos2 - 1).c_str());
                avpName = avp_name.substr(start_pos, pos2-start_pos);;
            }
            else
            {
                avpName = avp_name.substr(start_pos, 500);
            }
            quit_flag = 1;
        }

        //	printf("avpName:[%s]\n", avpName.c_str());
        if ((c = acl->search(avpName.c_str())))
        {
            if (c->size() > pos)
            {
                if (((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE)
                {
                    acl = ((*c)[pos])->dataPtr(Type2Type < AAAAvpContainerList > ());
                    //					printf("avpName=%s,acl releaseContainers\n", avpName.c_str());
                    acl->releaseContainers();
                    pos = 0;
                    c->resize(0);
                }
                else
                {
                    //					printf("avpName=%s,c releaseEntries\n", avpName.c_str());
                    c->releaseEntries();
                    c->resize(0);
                    //					GET_DATA_REF(T, tmp, (*c)[pos]);
                    //					data = tmp;
                    pos = 0;
                    return 1;
                }
            }
            else
            {
#ifdef DEBUG_DISP
                printf("pos[%d] is large size[%d]\n", pos, c->size());
#endif
                return  - 1;
            }
        }
        else
        {
#ifdef DEBUG_DISP
            printf("avpName[%s] not found\n", avpName.c_str());
#endif
            return  - 1;
        }
    }
    return 0;

}


template<class T>
int OcpMsgParser::_getAvpValue(char *avp_name1, T &data)
{
    AAAAvpContainer *c;
    int quit_flag = 0;
    std::string::size_type pos, start_pos;
    std::string avpName, avp_name = std::string(avp_name1);
    std::string::size_type pos1, pos2, pos3;
    AAAAvpContainerList *acl = &m_oMsg->acl;
    for (pos = 0, start_pos = 0; !quit_flag;)
    {
        pos1 = avp_name.find(".", start_pos);
        if (pos1 != std::string::npos)
        {
            avpName = avp_name.substr(start_pos, pos1 - start_pos);
            pos2 = avpName.find("[", 0);
            if (pos2 != std::string::npos)
            {
                pos3 = avpName.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avpName.substr(pos2 + 1, pos3 - pos2 - 1).c_str());
                avpName.erase(pos2, 10);
            }
            start_pos = pos1 + 1;
        }
        else
        {
            pos2 = avp_name.find("[", start_pos);
            if (pos2 != std::string::npos)
            {
                pos3 = avp_name.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avp_name.substr(pos2+1, pos3-pos2-1).c_str());
                avpName = avp_name.substr(start_pos, pos2-start_pos);;
            }
            else
            {
                avpName = avp_name.substr(start_pos, 500);
            }
            quit_flag = 1;
        }

        //	printf("avpName:[%s]\n", avpName.c_str());
        if ((c = acl->search(avpName.c_str())))
        {
            if (c->size() > pos)
            {
                if (((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE)
                {
                    acl = ((*c)[pos])->dataPtr(Type2Type<AAAAvpContainerList>());
                    pos = 0;
                }
                else
                {
                    GET_DATA_REF(T, tmp, (*c)[pos]);
                    data = tmp;
                    pos = 0;
                    return 1;
                }
            }
            else
            {
#ifdef DEBUG_DISP
                printf("pos[%d] is large size[%d]\n", pos, c->size());
#endif
                return  - 1;
            }
        }
        else
        {
#ifdef DEBUG_DISP
            printf("avpName[%s] not found\n", avpName.c_str());
#endif
            return  - 1;
        }
    }
    return 0;

}

template<class T>
int  OcpMsgParser::_setAvpValue( char *avp_name1, T &data, AAA_AVPDataType t)
{
    AAAAvpContainerManager cm;
    AAAAvpContainerEntryManager em;
    AAAAvpContainerEntry *e;
    int quit_flag = 0, ret = 0, flag, set_flag;
    std::string::size_type pos, start_pos;
    std::string avpName, avp_name = std::string(avp_name1);
    std::string::size_type pos1, pos2, pos3;
    AAAAvpContainerList *acl = &m_oMsg->acl;
    AAAAvpContainerList *tmp_list = NULL;
    AAAAvpContainer *c;
    for (pos = 0, start_pos = 0, flag = 0, set_flag = 0; !quit_flag;)
    {
        pos1 = avp_name.find(".", start_pos);
        if (pos1 != std::string::npos)
        {
            avpName = avp_name.substr(start_pos, pos1 - start_pos);
            pos2 = avpName.find("[", 0);
            if (pos2 != std::string::npos)
            {
                pos3 = avpName.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avpName.substr(pos2 + 1, pos3 - pos2 - 1).c_str());
                avpName.erase(pos2, 10);

            }

            start_pos = pos1 + 1;
        }
        else
        {
            pos2 = avp_name.find("[", start_pos);
            if (pos2 != std::string::npos)
            {
                pos3 = avp_name.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avp_name.substr(pos2+1, pos3-pos2-1).c_str());
                avpName = avp_name.substr(start_pos, pos2-start_pos);;
            }
            else
            {
                avpName = avp_name.substr(start_pos, 500);
            }
            quit_flag = 1;
        }
        //	printf("avpName:[%s]\n", avpName.c_str());
        if (!flag && (c = acl->search(avpName.c_str())))
        {
            //		printf("1.avpName:[%s]\n", avpName.c_str());
            if (c->size() > pos)
            {
                if (((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE)
                {
                    acl = ((*c)[pos])->dataPtr(Type2Type<AAAAvpContainerList>());
                    pos = 0;
                    set_flag = 1;
                    //				printf("2.avpName:[%s],set_flag=1\n", avpName.c_str());
                }
                else
                {
                    //				printf("3.avpName:[%s],set_value\n", avpName.c_str());
                    if (t != AAA_AVP_DATA_TYPE)
                    {
                        GET_DATA_REF(T, tmp, (*c)[pos]);
                        pos = 0;
                        tmp = data;
                    }
                    setSetFlag(true);
                    return 1;
                }
            }
            else
            {
                //   		printf("pos[%d] is large size[%d]\n", pos, c->size());
                if (!quit_flag)
                {
                    //				printf("11.avpName:[%s],set_flag\n", avpName.c_str());
                    e = em.acquire(AAA_AVP_GROUPED_TYPE);
                    acl = e->dataPtr(Type2Type < diameter_grouped_t > ());
                    c->add(e);

                }
                else
                {
                    if (t != AAA_AVP_DATA_TYPE)
                    {
                        e = em.acquire(t);
                        GET_DATA_REF(T, tmp, e);
                        c->add(e);
                        tmp = data;
                    }
                    ret = 1;
                }
                flag = 1;
            }
        }
        else
        {


            //		printf("2.avpName:[%s]\n", avpName.c_str());
            c = cm.acquire(avpName.c_str());
            if (!quit_flag)
            {
                //			printf("5.avpName:[%s],set_flag\n", avpName.c_str());
                e = em.acquire(AAA_AVP_GROUPED_TYPE);
                tmp_list = e->dataPtr(Type2Type < diameter_grouped_t > ());
                c->add(e);
            }
            else
            {
                //			printf("4.avpName:[%s],set_value\n", avpName.c_str());
                //			cout << "data:t" << t << ":" << data << endl;
                if (t != AAA_AVP_DATA_TYPE)
                {
                    e = em.acquire(t);
                    GET_DATA_REF(T, tmp, e);
                    c->add(e);
                    tmp = data;
                    //			cout << "data:" << data << ":" << tmp << endl;
                }
                ret = 1;
            }

            if (acl)
            {
                //	  	printf("2.acl add\n");
                acl->add(c);
            }
            acl = tmp_list;
            flag = 1;
        }

    }
    if (ret)
        setSetFlag(true);
    return ret;
}


int OcpMsgParser::getDiameterCommandCode()
{
    if (m_oMsg->hdr.flags.r == AAA_FLG_SET)
    {
        switch (m_oMsg->hdr.code)
        {
        case COMMAND_CODE_CC:
            return CCR1;
        case COMMAND_CODE_RA:
            return RAR;
        case COMMAND_CODE_AS:
            return ASR;
        case COMMAND_CODE_DW:
            return DWR;
        case COMMAND_CODE_DP:
            return DPR;
        case COMMAND_CODE_ST:
            return STR;
        case COMMAND_CODE_CE:
            return CER;
        default:
            return NOKNOWN;
        }
    }
    else
    {
        switch (m_oMsg->hdr.code)
        {
        case COMMAND_CODE_CC:
            return CCA;
        case COMMAND_CODE_RA:
            return RAA;
        case COMMAND_CODE_AS:
            return ASA;
        case COMMAND_CODE_DW:
            return DWA;
        case COMMAND_CODE_DP:
            return DPA;
        case COMMAND_CODE_ST:
            return STA;
        case COMMAND_CODE_CE:
            return CEA;
        default:
            return NOKNOWN;
        }
    }
}


int OcpMsgParser::setAvpValue(char *avp_name, int &data)
{
    ACE_INT32 tmp_data = static_cast < ACE_INT32 > (data);
    return _setAvpValue(avp_name, tmp_data, AAA_AVP_INTEGER32_TYPE);
}

int OcpMsgParser::setAvpValue(char *avp_name, ACE_UINT32 &data)
{
    ACE_UINT32 tmp_data = static_cast < ACE_UINT32 > (data);
    return _setAvpValue(avp_name, tmp_data, AAA_AVP_UINTEGER32_TYPE);
}

int OcpMsgParser::setAvpValue(char *avp_name, long &data)
{
    ACE_INT32 tmp_data = static_cast < ACE_INT32 > (data);
    return _setAvpValue(avp_name, tmp_data, AAA_AVP_INTEGER32_TYPE);
}

int OcpMsgParser::setAvpValue(char *avp_name, ACE_UINT64 &data)
{
    /*
    ACE_UINT64 int64x = data;
    unsigned char *ptr1 = (unsigned char *)&data;
    unsigned char *ptr2 = (unsigned char *)&int64x;
    int m_iLen = sizeof(ACE_UINT64);
    for (int i=0,j=m_iLen;i<m_iLen;i++)
    {
    ptr1[i] = ptr2[--j];
    }

    int ret = _setAvpValue( avp_name, data, AAA_AVP_UINTEGER64_TYPE);
    data = int64x;
    return ret;
     */
    return _setAvpValue(avp_name, data, AAA_AVP_UINTEGER64_TYPE);
}

int OcpMsgParser::setAvpValue(char *avp_name, diameter_address_t &data)
{
    return _setAvpValue(avp_name, data, AAA_AVP_ADDRESS_TYPE);
}


int OcpMsgParser::setAvpValue(char *avp_name, std::string &data)
{
    return _setAvpValue(avp_name, data, AAA_AVP_STRING_TYPE);
}

int OcpMsgParser::setAvpValue(char *avp_name)
{
    std::string data;
    char tmp_name[200];
    sprintf(tmp_name, "%s.AVP", avp_name);
    return _setAvpValue(tmp_name, data, AAA_AVP_DATA_TYPE);
}


int OcpMsgParser::setAvpValue(char *avp_name, float &data)
{
    ACE_UINT64 unit_digits;
    ACE_INT32 exponent;
    //	int 	unit_digits;
    //	int	exponent;
    float data1 = data;
    int i;

    char tmp_avp_name[300],  *p;
    sprintf(tmp_avp_name, "%f", data);
    //	printf("tmp_avp_name=%s\n", tmp_avp_name);
    if ((p = (char*)strstr(tmp_avp_name, ".")))
    {
        exponent = strlen(p + 1);
        for (i = 0; i < exponent; i++)
            data1 *= 10;
        unit_digits = (ACE_UINT64)data1;
        //		unit_digits = (int) data1;
#ifdef DEBUG_DISP
        std::cout << "p=" << p + 1 << ":1.unit_digits=" << unit_digits << ",exponent=" << exponent << std::endl;
#endif
    }
    else
    {
        unit_digits = (ACE_UINT64)data;
        //		unit_digits = (int) data;
        exponent = 0;
#ifdef DEBUG_DISP
        std::cout << "2.unit_digits=" << unit_digits << ",exponent=" << exponent << std::endl;
#endif
    }

    //	std::cout << "3.unit_digits=" << unit_digits << ",exponent=" << exponent << std::endl;
    sprintf(tmp_avp_name, "%s.Value-Digits", avp_name);
    if (_setAvpValue(tmp_avp_name, unit_digits, AAA_AVP_INTEGER64_TYPE) == 0)
        return 0;
    if (exponent == 0)
        return 1;
    sprintf(tmp_avp_name, "%s.Exponent", avp_name);
    if (_setAvpValue(tmp_avp_name, exponent, AAA_AVP_INTEGER32_TYPE) == 0)
        return 0;
    return 1;
}



int OcpMsgParser::getAvpValue(char *avp_name, int &data)
{
    ACE_INT32 tmp_data;
    data = 0;
    int ret = _getAvpValue(avp_name, tmp_data);
    if (ret > 0)
        data = static_cast < int > (tmp_data);
    return ret;
}

int OcpMsgParser::getAvpValue(char *avp_name, long &data)
{
    ACE_INT32 tmp_data;
    data = 0L;
    int ret = _getAvpValue(avp_name, tmp_data);
    if (ret > 0)
        data = static_cast < long > (tmp_data);
    return ret;
}

int OcpMsgParser::getAvpValue(char *avp_name, ACE_UINT32 &data)
{
    ACE_UINT32 tmp_data;
    data = 0;
    int ret = _getAvpValue(avp_name, tmp_data);
    if (ret > 0)
        data = static_cast < long > (tmp_data);
    return ret;
}


int OcpMsgParser::getAvpValue(char *avp_name, ACE_UINT64 &data)
{
    data = 0;
    /*
    cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
    cout<<"avp_name="<<avp_name<<endl;
    data = 0;
    int ret = _getAvpValue( avp_name, data);
    cout<<"avp_name"<<data<<endl;
    ACE_UINT64 int64x = data;
    unsigned char *ptr1 = (unsigned char *)&data;
    unsigned char *ptr2 = (unsigned char *)&int64x;
    int m_iLen = sizeof(ACE_UINT64);
    for (int i=0,j=m_iLen;i<m_iLen;i++)
    {
    ptr1[i] = ptr2[--j];
    cout<<"ptr1["<<i<<"]="<<ptr1[i]<<" ptr2["<<j<<"]="<<ptr2[--j]<<"]"<<endl;
    }

    cout<<"avp_name"<<data<<endl;

    cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"<<endl;
    return ret;
     */
    /*//modify xupy 2008-09-26
    int ret = _getAvpValue( avp_name, data);

    ACE_UINT64 int64x = data;
    unsigned char *ptr1 = (unsigned char*) &data;
    unsigned char *ptr2 = (unsigned char*) &int64x;
    int m_iLen = sizeof(ACE_UINT64);
    for (int i = 0, j = m_iLen; i < m_iLen; i++)
    {
      ptr1[i] = ptr2[--j];
    }

    return ret;
    */
    return _getAvpValue( avp_name, data);
}


int OcpMsgParser::getAvpValue(char *avp_name, std::string &data)
{
    data = "";
    return _getAvpValue(avp_name, data);
}

int OcpMsgParser::getAvpValue(char *avp_name, diameter_address_t &data)
{
    return _getAvpValue(avp_name, data);
}

int OcpMsgParser::getAvpValue(char *avp_name, float &data)
{
    data = 0.0f;
    ACE_UINT64 unit_digits;
    ACE_INT32 exponent;
    int i;
    char tmp_avp_name[300];
    sprintf(tmp_avp_name, "%s.Value-Digits", avp_name);
    if (_getAvpValue(tmp_avp_name, unit_digits) == 0)
        return 0;
    sprintf(tmp_avp_name, "%s.Exponent", avp_name);
    if (_getAvpValue(tmp_avp_name, exponent) == 0)
    {
        data = (float)unit_digits;
        return 0;
    }
    data = (float)unit_digits;
    for (i = 0; i < exponent; i++)
        data /= 10;
    // 	printf("data=%f\n", data);
    return 1;
}


int OcpMsgParser::getGroupedAvpNums1(const char *avp_name1)
{
    AAAAvpContainer *c;
    char tmp_name[300], avp_name[300],  *p;
    int i, flag;
    AAAAvpContainerList *acl = &m_oMsg->acl;
    strcpy(avp_name, avp_name1);
    for (i = 0, flag = 0;; i++)
    {
        //		if( (p = (char *) ACE_OS::strstr(avp_name, ".")) )
        if ((p = (char*)strstr(avp_name, ".")))
        {
            memset(tmp_name, 0, 200);
            memcpy(tmp_name, avp_name, p - avp_name);
            strcpy(avp_name, tmp_name);
            if ((c = acl->search(tmp_name)))
            {
                if (((*c)[0]->dataType()) == AAA_AVP_GROUPED_TYPE)
                {
                    acl = ((*c)[0])->dataPtr(Type2Type < AAAAvpContainerList > ());
                    strcpy(tmp_name, p + 1);
                    strcpy(avp_name, tmp_name);
                }
            }
        }
        else
        {
            if ((c = acl->search(avp_name)))
            {
//        if (((*c)[0]->dataType()) == AAA_AVP_GROUPED_TYPE)
                {
                    return c->size();
                }
            }
            break;
        }
    }
    return 0;
}

int OcpMsgParser::getGroupedAvpNums(const char *avp_name1)
{
    AAAAvpContainer *c;
    int quit_flag = 0;
    std::string::size_type pos, start_pos;
    std::string avpName, avp_name = std::string(avp_name1);
    std::string::size_type pos1, pos2, pos3;
    AAAAvpContainerList *acl = &m_oMsg->acl;
    for (pos = 0, start_pos = 0; !quit_flag;)
    {
        pos1 = avp_name.find(".", start_pos);
        if (pos1 != std::string::npos)
        {
            avpName = avp_name.substr(start_pos, pos1 - start_pos);
            pos2 = avpName.find("[", 0);
            if (pos2 != std::string::npos)
            {
                pos3 = avpName.find("]", pos2);
                if (pos3 != std::string::npos)
                    pos = atoi(avpName.substr(pos2 + 1, pos3 - pos2 - 1).c_str());
                avpName.erase(pos2, 10);
            }
            start_pos = pos1 + 1;
        }
        else
        {
            avpName = avp_name.substr(start_pos, 500);;
            quit_flag = 1;
        }

        //	printf("avpName:[%s]\n", avpName.c_str());
        if ((c = acl->search(avpName.c_str())))
        {
            if (c->size() > pos)
            {
                if (((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE)
                {
                    acl = ((*c)[pos])->dataPtr(Type2Type < AAAAvpContainerList > ());
                    pos = 0;
                    if (quit_flag)
                        return c->size();
                }
                /*				else
                {
                GET_DATA_REF(T, tmp, (*c)[pos]);
                data = tmp;
                pos = 0;
                return 1;
                }
                 */
            }
            else
            {
#ifdef DEBUG_DISP
                printf("pos[%d] is large size[%d]\n", pos, c->size());
#endif
                return  - 1;
            }
        }
        else
        {

#ifdef DEBUG_DISP
            printf("avpName[%s] not found\n", avpName.c_str());
#endif

            return  - 1;
        }
    }
    return 0;

}


void OcpMsgParser::getAvpName(std::vector<std::string>& sAvpName, std::string& avp_name)
{
    std::vector < std::string > ::iterator itor;

    for (itor = sAvpName.begin(); itor != sAvpName.end(); itor++)
    {
        if (itor == sAvpName.begin())
            avp_name = (*itor);
        else
            avp_name += "." + (*itor);

    }
}

void OcpMsgParser::setOutStr(std::string& avp_name, char *sFormatStr, char *sTmpBuf)
{
    if (avp_name.find("LCOCS", 0) == std::string::npos)
        m_sAvpOutStr += avp_name + sFormatStr + sTmpBuf + "\n";
}


void OcpMsgParser::getAvpData(AAAAvpContainerEntry *e, int pos, int size, std::vector<std::string>& sAvpName)
{
    std::string tmp;
    std::string avp_name = "";
    char sTmpBuf[512], sFormatStr[80];
    char sTmpValue[1024] = {0};
    int i;
    if (size > 1)
        sprintf(sFormatStr, "[%d]:", pos);
    else
        strcpy(sFormatStr, ":");
    switch (e->dataType())
    {
    case AAA_AVP_INTEGER32_TYPE:
    {
        //		  		to_string(tmp, e->dataRef(Type2Type<diameter_integer32_t>()) );
        sprintf(sTmpBuf, "%ld", e->dataRef(Type2Type<diameter_integer32_t>()));
        getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            std::cout <<"[type=diameter_integer32_t]"<< avp_name << sFormatStr << std::string(sTmpBuf) << std::endl;
            //Log::log(0,"[type=diameter_integer32_t]%s%s%s",avp_name.c_str(),sFormatStr,sTmpBuf);
            memset(sTmpValue, 0, 1024);
            sprintf(sTmpValue, "[type=diameter_integer32_t]%s%s%s\n",avp_name.c_str(),sFormatStr,sTmpBuf);
            m_sAvpOutStr += sTmpValue;
        }
        //					  mapInt32[avp_name] = e->dataRef(Type2Type<diameter_integer32_t>());
        //setOutStr(avp_name, sFormatStr, sTmpBuf);
    }
    break;
    case AAA_AVP_UINTEGER32_TYPE:
    {
        //		  		to_string(tmp, e->dataRef(Type2Type<diameter_unsigned32_t>()) );
        sprintf(sTmpBuf, "%lu", e->dataRef(Type2Type<diameter_unsigned32_t>()));
        getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            std::cout <<"[type=diameter_unsigned32_t]"<< avp_name << sFormatStr << std::string(sTmpBuf) << std::endl;
            //Log::log(0,"[type=diameter_unsigned32_t]%s%s%s",avp_name.c_str(),sFormatStr,sTmpBuf);
            memset(sTmpValue, 0, 1024);
            sprintf(sTmpValue, "[type=diameter_unsigned32_t]%s%s%s\n",avp_name.c_str(),sFormatStr,sTmpBuf);
            m_sAvpOutStr += sTmpValue;
        }
        //						mapInt32[avp_name] = e->dataRef(Type2Type<diameter_unsigned32_t>());
        //setOutStr(avp_name, sFormatStr, sTmpBuf);
    }
    break;

    case AAA_AVP_INTEGER64_TYPE:
    {
        //		  			to_string(tmp, e->dataRef(Type2Type<diameter_integer64_t>()) );
        sprintf(sTmpBuf, "%ld", e->dataRef(Type2Type<diameter_integer64_t>()));
        getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            std::cout <<"[type=diameter_integer64_t]"<< avp_name << sFormatStr << sTmpBuf << std::endl;
            //Log::log(0,"[type=diameter_integer64_t]%s%s%s",avp_name.c_str(),sFormatStr,sTmpBuf);
            memset(sTmpValue, 0, 1024);
            sprintf(sTmpValue, "[type=diameter_integer64_t]%s%s%s\n",avp_name.c_str(),sFormatStr,sTmpBuf);
            m_sAvpOutStr += sTmpValue;
        }
        //setOutStr(avp_name, sFormatStr, sTmpBuf);
        //						mapInt64[avp_name] = e->dataRef(Type2Type<diameter_integer64_t>());
    }
    break;
    case AAA_AVP_UINTEGER64_TYPE:
    {
        //	  		to_string(tmp, e->dataRef(Type2Type<diameter_unsigned64_t>()) );
        sprintf(sTmpBuf, "%lu", e->dataRef(Type2Type<diameter_unsigned64_t>()));
        getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            std::cout <<"[type=diameter_unsigned64_t]"<< avp_name << sFormatStr << sTmpBuf << std::endl;
            //Log::log(0,"[type=diameter_unsigned64_t]%s%s%s",avp_name.c_str(),sFormatStr,sTmpBuf);
            memset(sTmpValue, 0, 1024);
            sprintf(sTmpValue, "[type=diameter_unsigned64_t]%s%s%s\n",avp_name.c_str(),sFormatStr,sTmpBuf);
            m_sAvpOutStr += sTmpValue;
        }
        //setOutStr(avp_name, sFormatStr, sTmpBuf);
        //						mapInt64[avp_name] = e->dataRef(Type2Type<diameter_unsigned64_t>());
    }
    break;
    case AAA_AVP_ENUM_TYPE:
    {
        // to_string(tmp, e->dataRef(Type2Type<diameter_integer32_t>()) );
        sprintf(sTmpBuf, "%d", e->dataRef(Type2Type<diameter_integer32_t>()));
        getAvpName(sAvpName, avp_name);
        if (m_bDispFlag)
        {
            std::cout <<"[type=diameter_integer32_t]"<< avp_name << sFormatStr << std::string(sTmpBuf) << std::endl;
            //Log::log(0,"[type=diameter_integer32_t]%s%s%s",avp_name.c_str(),sFormatStr,sTmpBuf);
            memset(sTmpValue, 0, 1024);
            sprintf(sTmpValue, "[type=diameter_integer32_t]%s%s%s\n",avp_name.c_str(),sFormatStr,sTmpBuf);
            m_sAvpOutStr += sTmpValue;
        }
        //						mapInt32[avp_name] = e->dataRef(Type2Type<diameter_integer32_t>());
        //setOutStr(avp_name, sFormatStr, sTmpBuf);
    }
    break;
    case AAA_AVP_STRING_TYPE:
    {
        getAvpName(sAvpName, avp_name);
        strcpy(sTmpBuf, e->dataRef(Type2Type<diameter_octetstring_t>()).c_str());
        if (avp_name.find("LCOCS-PayAcctItemList", 0) != std::string::npos ||
                avp_name.find("LCOCS-HoldAcctItemList", 0) != std::string::npos)
        {
            tmp = e->dataRef(Type2Type < diameter_octetstring_t > ());
            if (m_bDispFlag)
            {
                std::cout <<"[type=diameter_octetstring_t]"<< avp_name << sFormatStr << "length:[" << tmp.size()<< "]" << std::endl;
                //Log::log(0,"[type=diameter_octetstring_t]%s%slength:[%d]",avp_name.c_str(),sFormatStr,tmp.size());
                memset(sTmpValue, 0, 1024);
                sprintf(sTmpValue, "[type=diameter_octetstring_t]%s%slength:[%d]\n",avp_name.c_str(),sFormatStr,sTmpBuf);
                m_sAvpOutStr += sTmpValue;
            }
        }
        else
        {
            int i;
            if (m_bDispFlag)
            {
                tmp = e->dataRef(Type2Type < diameter_octetstring_t > ());
                //strcpy(sTmpBuf, tmp1.c_str());
                std::cout <<"[type=diameter_octetstring_t]"<< avp_name << sFormatStr ;
                //Log::log(0,"[type=diameter_octetstring_t]%s%s",avp_name.c_str(),sFormatStr);
                memset(sTmpValue, 0, 1024);
                sprintf(sTmpValue, "[type=diameter_octetstring_t]%s%s\n",avp_name.c_str(),sFormatStr,sTmpBuf);
                m_sAvpOutStr += sTmpValue;
                for (i = 0; i < tmp.size(); i++)
                {
                    if (isprint(tmp[i]))
                        printf("%c", tmp[i]);
                    else
                        printf("\\x%02X", tmp[i] &0xFF);
                }
                printf("\n");
            }
            //   << sFormatStr <<  << std::endl;
        }
        //setOutStr(avp_name, sFormatStr, sTmpBuf);
        //						mapStr[avp_name] = e->dataRef(Type2Type<diameter_octetstring_t>());
    }
    break;

    case AAA_AVP_ADDRESS_TYPE:
    {
        getAvpName(sAvpName, avp_name);
        diameter_address_t &addr = e->dataRef(Type2Type<diameter_address_t>());
        sprintf(sTmpBuf, "%d:", addr.type);
        int len = addr.value.length();
        unsigned char tmp[100];
        memcpy(tmp, addr.value.c_str(), len);
        for (i = 0; i < len; i++)
            sprintf(sTmpBuf + strlen(sTmpBuf), "%d.", tmp[i]);

        if (sTmpBuf[strlen(sTmpBuf) - 1] == '.')
            sTmpBuf[strlen(sTmpBuf) - 1] = 0;

        if (m_bDispFlag)
        {
            std::cout <<"[type=diameter_address_t]"<< avp_name << sFormatStr << sTmpBuf << std::endl;
            //Log::log(0,"[type=diameter_address_t]%s%s%s",avp_name.c_str(),sFormatStr,sTmpBuf);
            memset(sTmpValue, 0, 1024);
            sprintf(sTmpValue, "[type=diameter_address_t]%s%s%s\n",avp_name.c_str(),sFormatStr,sTmpBuf);
            m_sAvpOutStr += sTmpValue;
        }
        //setOutStr(avp_name, sFormatStr, sTmpBuf);
    }
    break;
    case AAA_AVP_UTF8_STRING_TYPE:
    {
        getAvpName(sAvpName, avp_name);
        strcpy( sTmpBuf, e->dataRef(Type2Type<diameter_utf8string_t>()).c_str());
        if (m_bDispFlag)
        {
            std::cout <<"[type=diameter_utf8string_t]"<< avp_name << sFormatStr << e->dataRef(Type2Type<diameter_utf8string_t>()) << std::endl;
            //Log::log(0,"[type=diameter_utf8string_t]%s%s%s",avp_name.c_str(),sFormatStr,e->dataRef(Type2Type<diameter_utf8string_t>()).c_str());
            memset(sTmpValue, 0, 1024);
            sprintf(sTmpValue, "[type=diameter_utf8string_t]%s%s%s\n",avp_name.c_str(),sFormatStr,sTmpBuf);
            m_sAvpOutStr += sTmpValue;
        }
        //			    mapStr[avp_name] = e->dataRef(Type2Type<diameter_octetstring_t>());
        //setOutStr(avp_name, sFormatStr, sTmpBuf);
    }
    break;
    case AAA_AVP_DIAMID_TYPE:
    {
        getAvpName(sAvpName, avp_name);
        strcpy( sTmpBuf, e->dataRef(Type2Type<diameter_identity_t>()).c_str());
        if (m_bDispFlag)
        {
            std::cout <<"[type=diameter_identity_t]"<< avp_name << sFormatStr << e->dataRef(Type2Type<diameter_identity_t>()) << std::endl;
            //Log::log(0,"[type=diameter_identity_t]%s%s%s",avp_name.c_str(),sFormatStr,e->dataRef(Type2Type<diameter_identity_t>()).c_str());
            memset(sTmpValue, 0, 1024);
            sprintf(sTmpValue, "[type=diameter_identity_t]%s%s%s\n",avp_name.c_str(),sFormatStr,sTmpBuf);
            m_sAvpOutStr += sTmpValue;
        }
        //				    mapStr[avp_name] = e->dataRef(Type2Type<diameter_octetstring_t>());
        //setOutStr(avp_name, sFormatStr, sTmpBuf);
    }
    break;
    case AAA_AVP_DATA_TYPE:
    case AAA_AVP_DIAMURI_TYPE:
    case AAA_AVP_TIME_TYPE:
    case AAA_AVP_GROUPED_TYPE:
    case AAA_AVP_IPFILTER_RULE_TYPE:
    case AAA_AVP_CUSTOM_TYPE:

        //2008/11/19 ZC
        if (m_bDispFlag)
        {
            std::cout <<"WARNING TYPE [type=diameter_identity_t]"<< avp_name << sFormatStr << e->dataRef(Type2Type<diameter_identity_t>()) << std::endl;
            //Log::log(0,"WARNING TYPE [type=diameter_identity_t]%s%s%s",avp_name.c_str(),sFormatStr,e->dataRef(Type2Type<diameter_identity_t>()).c_str());
            memset(sTmpValue, 0, 1024);
            sprintf(sTmpValue, "WARNING TYPE [type=diameter_identity_t]%s%s%s\n",avp_name.c_str(),sFormatStr,sTmpBuf);
            m_sAvpOutStr += sTmpValue;

        }

        break;
    }

}


void OcpMsgParser::_getAllAvp(AAAAvpContainerList *orig_acl, std::vector<std::string>& sAvpName)
{
    AAAAvpContainer *c;
    AAAAvpContainerList *acl = orig_acl,  *acl1;
    AAAAvpContainerList::iterator itor;
    unsigned int i;
    std::string tmp;
    char tmp_buf[20];

    int m_iGroupNum = 0; //zc add  ##################
    int mi_AvpNum = 0;

    for (itor = acl->begin(); itor != acl->end(); itor++)
    {

        c =  *itor;
        for (i = 0; i < c->size(); i++)
        {

            if (((*c)[i]->dataType()) == AAA_AVP_GROUPED_TYPE)
            {
                m_iGroupNum++; //zc add  ##################
                if (c->size() > 1)
                {
                    sprintf(tmp_buf, "%d", i);
                    tmp = c->getAvpName() + std::string("[") + std::string(tmp_buf) + std::string("]");
                    sAvpName.push_back(tmp);
                }
                else
                {
                    if (strstr(c->getAvpName(), "Requested-Service-Unit") &&
                            ( sAvpName.size() > 0 &&  strstr(sAvpName.back().c_str(), "Multiple-Services-Credit-Control")) )
                    {
                        std::string avp_name = "";
                        sAvpName.push_back(c->getAvpName());
                        getAvpName(sAvpName, avp_name);
                        sAvpName.pop_back();
                        if (m_bDispFlag)
                            std::cout << avp_name << ":" << std::endl;
                        //m_sAvpOutStr += avp_name + ":" + "\n";
                    }
                    sAvpName.push_back(c->getAvpName());
                }
                acl1 = ((*c)[i])->dataPtr(Type2Type < AAAAvpContainerList > ());
                _getAllAvp(acl1, sAvpName);
                sAvpName.pop_back();
            }
            else
            {
                mi_AvpNum++;
                sAvpName.push_back(c->getAvpName());
                getAvpData((*c)[i], i, c->size(), sAvpName);
                sAvpName.pop_back();

            }
        }

        /*  	if( c->size() == 0 )
        {
        std::string avp_name="";
        sAvpName.push_back(c->getAvpName());
        getAvpName(sAvpName, avp_name);
        if( m_bDispFlag )
        std::cout << avp_name << ":" << std::endl;
        m_sAvpOutStr += avp_name + ":" + "\n";
        sAvpName.pop_back();
        }
         */
    }
}

std::string &OcpMsgParser::getAllAvp(bool disp_flag)
{
    AAAAvpContainerList *acl = &m_oMsg->acl;
    std::vector < std::string > sAvpName;
    m_sAvpOutStr = std::string("");
    m_bDispFlag = disp_flag;
    if (m_bDispFlag)
        printHeader();

    _getAllAvp(acl, sAvpName);

    if (m_bDispFlag)
    {
        for (int i = 0; i < sAvpName.size(); i++)
        {
            cout << "sAvpName[" << i << "]=" << sAvpName[i] << endl;
            //Log::log(0,"sAvpName[%d] = %s",sAvpName[i].c_str());
        }
    }
    return m_sAvpOutStr;
}

int OcpMsgParser::parseAppToRawWithCommandFail(int code, unsigned char *buf)
{
    unsigned char *p;
    int msg_len;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 0x20;
    buf[4] &= 0x7F;
    memcpy(&buf[20], "\x00\x00\x01\x0C\x40\x00\x00\x0C", 8);
    p = &buf[28];

    //  *((ACE_UINT32*)(p)) = ntohl(code);
    ACE_INT32 int32 = ntohl(code);
    memcpy(p, &int32, sizeof(ACE_INT32));
    msg_len = 32;
    return msg_len;
    //  disp_buf(buf, 32);
}

void OcpMsgParser::setAvpCheck(int avp_check)
{
    ::setAvpCheck(avp_check);
}


string OcpMsgParser::getServiceType(string &serviceContextId)
{

    string scenario;
    string::size_type pos1, pos2;
    string tmp = serviceContextId;
    //strcpy(serviceType, "UNKNOWN");
    //strcpy(serviceType, serviceContextId.c_str());
    pos1 = serviceContextId.find(".", 0);
    pos2 = serviceContextId.find("@", 0);
    if (pos1 != string::npos && pos2 != string::npos)
    {
        if (pos2 > pos1)
            tmp = serviceContextId.substr(pos1 + 1, pos2 - pos1 - 1);
        else
            tmp = serviceContextId.substr(0, pos2);
        //strcpy(serviceType, tmp.c_str());
    }
    //只取@前面的
    tmp = serviceContextId.substr(0, pos2);
    return tmp;
}

string OcpMsgParser::getSessionid()
{
    return m_sSsessionid;
}

long OcpMsgParser::getHopByHop()
{
    return m_iHopByHop;
}


int delAvpNodeByName(AAAAvpContainerList * acl, const char * name)
{
    AAAAvpContainerList::iterator iter;

    for(iter = acl->begin(); iter != acl->end(); iter++)
    {
        AAAAvpContainer * c = *iter;
        cout << c->getAvpName() << endl;
        if(strcmp(c->getAvpName(), name) == 0)
        {
            c->releaseEntries();

            acl->erase(iter);
            delete c;
            return 0;
        }
    }
    return -1;
}

void OcpMsgParser::initLocalInfo(char *sOriginHost, char *sOriginRealm, char *sLocalIp)
{
    m_strOriginHost = sOriginHost;
    m_strOriginRealm = sOriginRealm;
    m_strLocalIp = sLocalIp;
}

static void transform(std::string &str)
{
    int i;
    char tmp[50];
    strcpy(tmp, str.c_str());
    for (i = 0; tmp[i] != 0; i++)
    {
        tmp[i] = toupper(tmp[i]);
    }
    str = tmp;
}
int OcpMsgParser::avp2str(string strAvp, string &strData, int &iType)
{
    int iRet = parseRawToApp(strAvp);
    if (iRet)
    {
        return -1;
    }
    strData.clear();
    string str;
    unsigned int uiTemp;
    int iNum;
    int i;
    char buf[128];
    int iTemp;
    string strSevciceContextId;
    m_iHopByHop = m_oMsg->hdr.hh;
    switch (m_oMsg->hdr.code)
    {
    case COMMAND_CODE_CC:
    {
        getAvpValue("Service-Context-Id", strSevciceContextId);
        getAvpValue("Session-Id", str);
        m_sSsessionid = str;
        if (m_oMsg->hdr.flags.r)
        {
            iType = CCR1;

            getAvpValue("Session-Id", str);
            strData.append(str);
            strData.append("|");

            str = getServiceType(strSevciceContextId);

            //modified by zhufengsheng 2011.03.17
            //取原始的前面部分不转换判断是否是激活的逻辑
            //查找Active看是否有匹配
            //transform(str);
            bool bIsActiveCC = false;
            //bool bIsActiveCC = strcmp("ACTIVE", str.c_str()) == 0;
            if(str.find("Active") != std::string::npos)
            {
                bIsActiveCC = true;
            }
            //modified by zhufengsheng 2011.03.17


            sprintf(buf, "%d|", m_oMsg->hdr.hh);
            strData.append(buf);
            sprintf(buf, "%d|", m_oMsg->hdr.ee);
            strData.append(buf);

            getAvpValue("Auth-Application-Id", uiTemp);
            sprintf(buf, "%d|", uiTemp);
            strData.append(buf);

            getAvpValue("CC-Request-Type", uiTemp);
            sprintf(buf, "%d|", uiTemp);
            strData.append(buf);

            getAvpValue("CC-Request-Number", uiTemp);
            sprintf(buf, "%d|", uiTemp);
            strData.append(buf);

            strData.append(strSevciceContextId);
            if (bIsActiveCC)  //激活包CCR
            {
                iNum =
                    getGroupedAvpNums1("Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id");
                for (i = 0; i < iNum; i++)
                {
                    sprintf(buf,
                            "Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[%d]",
                            i);
                    getAvpValue(buf, str);
                    strData.append("^^^");
                    strData.append(str);
                }
            }
            else        //业务CCR
            {
                iNum =
                    getGroupedAvpNums1("Service-Information.Data-Information.Data-Table-Simple-Row");
                for (i = 0; i < iNum; i++)
                {
                    sprintf(buf,
                            "Service-Information.Data-Information.Data-Table-Simple-Row[%d]",
                            i);
                    getAvpValue(buf, str);
                    strData.append("^^^");
                    strData.append(str);
                }
            }

        }
        else
        {
            iType = CCA;
            bool bIsActiveCC = strSevciceContextId.size() != 0;
            //CCA有Service-Context-Id ,业务CCA没有Service-Context-Id
            if(bIsActiveCC)   //激活包CCA
            {
                getAvpValue("Result-Code", uiTemp);
                sprintf(buf, "%d", uiTemp);
                strData = buf;

                getAvpValue("Service-Information.Service-Result-Code", iTemp);
                sprintf(buf, "|%d", iTemp);
                strData.append(buf);

            }
            else
            {
                getAvpValue("Session-Id", str);
                strData.append(str);
                iNum = getGroupedAvpNums("Service-Information");
                for(i = 0; i < iNum; i++)
                {
                    strData.append("^^^");
                    sprintf(buf, "Service-Information[%d].Service-Result-Code", i);
                    getAvpValue(buf, iTemp);
                    sprintf(buf, "%d|", iTemp);
                    strData.append(buf);

                    sprintf(buf, "Service-Information[%d].Para-Field-Result",  i);
                    getAvpValue(buf, str);
                    strData.append(str);
                }
            }
        }
        break;
    }
    case COMMAND_CODE_DW:
    {
        getAvpValue("Origin-State-Id", uiTemp);
        releaseMsgAcl();
        strData.clear();
        if (m_oMsg->hdr.flags.r)
        {
            iType = DWR;
            m_oMsg->hdr.flags.r = 0;
            setAvpValue("Origin-State-Id", uiTemp);
            uiTemp = 2001;
            setAvpValue("Result-Code", uiTemp);
            setAvpValue("Origin-Host", m_strOriginHost);
            setAvpValue("Origin-Realm", m_strOriginRealm);
            iRet = parseAppToRaw(strData);
        }
        else
        {
            iType = DWA;
        }
        break;
    }
    case COMMAND_CODE_DP:
    {
        if (m_oMsg->hdr.flags.r)
        {
            iType = DPR;
            releaseMsgAcl();
            m_oMsg->hdr.flags.r = 0;
            uiTemp = 2001;
            setAvpValue("Result-Code", uiTemp);
            setAvpValue("Origin-Host",m_strOriginHost);
            setAvpValue("Origin-Realm",m_strOriginRealm);
            iRet = parseAppToRaw(strData);
        }
        else
        {
            iType = DPA;
        }
        break;
    }
    case COMMAND_CODE_CE:
    {
        if (m_oMsg->hdr.flags.r)
        {
            iType = CER;
            int iVendorId = 0;
            string sProductName;

            getAvpValue("Origin-Host", m_strDestHost);
            getAvpValue("Origin-Realm", m_strDestRealm);

            getAvpValue("Vendor-Id", iVendorId);
            getAvpValue("Product-Name", sProductName);

            releaseMsgAcl();

            m_oMsg->hdr.flags.r = 0;
            uiTemp = 2001;
            setAvpValue("Result-Code", uiTemp);
            setAvpValue("Origin-Host",m_strOriginHost);
            setAvpValue("Origin-Realm",m_strOriginRealm);

            diameter_address_t m_dHostIPAddressX;
            m_dHostIPAddressX.type =1;

            char sHostName[1024] = {0};
            int i;
            struct in_addr addr;
            char LocalIp[64] = {0};

            // modified by zhangch
            strcpy(LocalIp, m_strLocalIp.c_str());
            /*if(gethostname(sHostName, sizeof(sHostName)) !=-1)
            {
                struct hostent* phost = gethostbyname(sHostName);
                for(i=0;phost!= NULL&&phost->h_addr_list[i]!=NULL;i++)
                {
                    memcpy(&addr, phost->h_addr_list[i], sizeof(struct in_addr));
                }
                strcpy(LocalIp, inet_ntoa(addr));
            }*/
            int buf[4] = {0};
            sscanf(LocalIp, "%d.%d.%d.%d", &buf[0], &buf[1], &buf[2], &buf[3]);
            char tmp[4] = {0};
            tmp[0] = buf[0];
            tmp[1] = buf[1];
            tmp[2] = buf[2];
            tmp[3] = buf[3];

            m_dHostIPAddressX.value.assign(tmp, 4);
            setAvpValue("Host-IP-Address", m_dHostIPAddressX);
            setAvpValue("Vendor-Id", iVendorId);
            setAvpValue("Product-Name", sProductName);

            iRet = parseAppToRaw(strData);
        }
        else
        {
            iType = CEA;
            getAvpValue("Result-Code", uiTemp);
            sprintf(buf, "%d", uiTemp);
            strData.append(buf);
        }
        break;
    }
    default:
    {
        return -1;
    }
    }
    return 0;
}

int OcpMsgParser::str2avp(int iType, string strData, string &strAvp)
{
    string str;
    unsigned int uiTemp;
    int iTemp;
    int pos1 = 0;
    int pos2 = 0;
    int hh;
    int ee;
    int i = 1;
    char buf[128] = {0};
    char buf1[128] = {0};
    releaseMsgAcl();
    switch (iType)
    {
    case CCR1:
    case CCA:
    {
        setAvpValue("Origin-Host", m_strOriginHost);
        setAvpValue("Origin-Realm", m_strOriginRealm);

        pos2 = strData.find("|");
        str = strData.substr(0, pos2);
        pos1 = pos2 + 1;
        setAvpValue("Session-Id", str);
        m_sSsessionid = str;

        pos2 = strData.find("|", pos1);
        str = strData.substr(pos1, pos2 - pos1);
        pos1 = pos2 + 1;
        sscanf(str.c_str(), "%d", &hh);


        pos2 = strData.find("|", pos1);
        str = strData.substr(pos1, pos2 - pos1);
        pos1 = pos2 + 1;
        sscanf(str.c_str(), "%d", &ee);


        pos2 = strData.find("|", pos1);
        str = strData.substr(pos1, pos2 - pos1);
        pos1 = pos2 + 1;
        sscanf(str.c_str(), "%d", &uiTemp);
        setAvpValue("Auth-Application-Id", uiTemp);


        pos2 = strData.find("|", pos1);
        str = strData.substr(pos1, pos2 - pos1);
        pos1 = pos2 + 1;
        sscanf(str.c_str(), "%d", &iTemp);
        setAvpValue("CC-Request-Type", iTemp);
        m_iHopByHop = hh;
        if (iType == CCR1)
        {
            setDiameterHeader(COMMAND_CODE_CC, true, 4, hh, ee);

            pos2 = strData.find("|", pos1);
            str = strData.substr(pos1, pos2 - pos1);
            pos1 = pos2 + 1;
            sscanf(str.c_str(), "%d", &iTemp);
            setAvpValue("CC-Request-Number", iTemp);

            pos2 = strData.find("|", pos1);
            str = strData.substr(pos1, pos2 - pos1);
            pos1 = pos2 + 1;
            setAvpValue("Service-Context-Id", str);

//{application_id}|{Destination-Realm}^^^话单1^^^...^^^话单n

            pos2 = strData.find("|", pos1);
            str = strData.substr(pos1, pos2 - pos1);
            pos1 = pos2 + 1;

//                setAvpValue("Destination-Host", str);


            pos2 = strData.find("^^^", pos1);
            str = strData.substr(pos1, pos2 - pos1);
            pos1 = pos2 + 3;
            setAvpValue("Destination-Realm", str);

            // ^^^话单1^^^...^^^话单n

            pos2 = strData.find("^^^", pos1);
            i = 0;
            while((unsigned int)pos1 < strData.size())
            {
                if (pos2 > 0)
                {
                    str = strData.substr(pos1, pos2 - pos1);
                    pos1 = pos2 + 3;
                }
                else
                {
                    str = strData.substr(pos1, strData.size() - pos1);
                    pos1 = strData.size();
                }

                sprintf(buf,
                        "Service-Information.Data-Information.Data-Table-Simple-Row[%d]",
                        i++);
                setAvpValue(buf, str);
                pos2 = strData.find("^^^", pos1);
            }

        }
        else
        {
            setDiameterHeader(COMMAND_CODE_CC, false, 4, hh, ee);
            uiTemp = 2001;
            setAvpValue("Result-Code", uiTemp);

            pos2 = strData.find("^^^", pos1);
            str = strData.substr(pos1, pos2 - pos1);
            pos1 = pos2 + 3;
            sscanf(str.c_str(), "%d", &iTemp);
            setAvpValue("CC-Request-Number", iTemp);


            // ^^^Service-Result-Code|Para-Field-Result^^^...^^^Service-Result-Code|Para-Field-Result
            pos2 = strData.find("^^^", pos1);
            i = 0;
            while((unsigned int)pos1 < strData.size())
            {
                if (pos2 > 0)
                {
                    str = strData.substr(pos1, pos2 - pos1);
                    pos1 = pos2 + 3;
                }
                else
                {
                    str = strData.substr(pos1, strData.size() - pos1);
                    pos1 = strData.size();
                }

                sscanf(str.c_str(), "%d|%s", &iTemp, buf1);
                sprintf(buf, "Service-Information[%d].Service-Result-Code", i);
                setAvpValue(buf, iTemp);
                sprintf(buf, "Service-Information[%d].Para-Field-Result", i++);
                str = buf1;
                setAvpValue(buf, str);
                pos2 = strData.find("^^^", pos1);
            }
        }
        m_oMsg->hdr.flags.p = 1;
        break;
    }
    default:
    {
        return -1;
    }
    }
    return parseAppToRaw(strAvp);
}

/*
生成CER包
	int createCER(char *sArguments, char *sCERbuf, int iCERSize);
	sArguments：传入参数
		形式：hh|ee|Vendor-Id|Product-Name
	sCERbuf：   打包后CER包
	iCERSize：  sCERbuf的大小

*/
int OcpMsgParser::createCER(char *sArguments, char *sCERbuf, int iCERSize)
{
    releaseMsgAcl();
    string str;
    int hh = 0;
    int ee = 0;
    int pos1 = 0;
    int pos2 = 0;

    string strData = sArguments;

    pos2 = strData.find("|");
    str = strData.substr(0, pos2);
    pos1 = pos2 + 1;
    sscanf(str.c_str(), "%d", &hh);

    pos2 = strData.find("|", pos1);
    str = strData.substr(pos1, pos2 - pos1);
    pos1 = pos2 + 1;
    sscanf(str.c_str(), "%d", &ee);


    setDiameterHeader(COMMAND_CODE_CE, true, 0, hh, ee);

    setAvpValue("Origin-Host", m_strOriginHost);
    setAvpValue("Origin-Realm", m_strOriginRealm);

    // add by zhangch
    diameter_address_t m_dHostIPAddressX;
    m_dHostIPAddressX.type =1;
    char LocalIp[64] = {0};

    strcpy(LocalIp, m_strLocalIp.c_str());
    int buf[4] = {0};
    sscanf(LocalIp, "%d.%d.%d.%d", &buf[0], &buf[1], &buf[2], &buf[3]);
    char tmp[4] = {0};
    tmp[0] = buf[0];
    tmp[1] = buf[1];
    tmp[2] = buf[2];
    tmp[3] = buf[3];

    m_dHostIPAddressX.value.assign(tmp, 4);
    // end add by zhangch

    setAvpValue("Host-IP-Address", m_dHostIPAddressX);




    unsigned int uiVendorId = 81000;
    string strProductName;

    pos2 = strData.find("|", pos1);
    str = strData.substr(pos1, pos2 - pos1);
    pos1 = pos2 + 1;
    sscanf(str.c_str(), "%d", &uiVendorId);

    setAvpValue("Vendor-Id", uiVendorId);


    strProductName = strData.substr(pos1, strData.size() - pos1);
    sscanf(str.c_str(), "%d", &uiVendorId);


    setAvpValue("Product-Name", strProductName);
    /*
           setAvpValue("Origin-State-Id", Origin_State_Id);
    	setAvpValue("Firmware-Revision", Firmware_Revision);
    */
    int iRet = parseAppToRaw(str);
    if (iRet < 0)
    {
        return -1;
    }
    if ((unsigned int)iCERSize < str.size())
    {
        return -1;
    }
    memcpy(sCERbuf, str.c_str(), str.size());
    return CER;
}

/*
生成激活CCR包
	int createActiveCCR( char *sArguments, char (*contextIDs)[256], int iRows, char *sCCRbuf, int iCCRSize);
	sArguments：传入参数
		形式：Session-Id|hh|ee|Destination-Realm|Auth-Application-Id|Service-Context-Id|CC-Request-Type|CC-Request-Number
	contextIDs：传入Service-Context-Id组，组内每条Service-Context-Id以'\0'结束
	iRows：       contextIDs的行数
	sCCRbuf：   打包后的激活CCR包
	iCCRSize：  sCCRbuf的大小

*/
int OcpMsgParser::createActiveCCR(char *sArguments, char (*contextIDs)[256], int iRows, char *sCCRbuf, int iCCRSize)
{
    string str;
    unsigned int uiTemp;
    int iTemp;
    int pos1 = 0;
    int pos2 = 0;
    int hh;
    int ee;
    int i = 0;
    char buf[128] = {0};
    char buf1[128] = {0};
    string strData = sArguments;
    releaseMsgAcl();

    setAvpValue("Origin-Host", m_strOriginHost);
    setAvpValue("Origin-Realm", m_strOriginRealm);

    pos2 = strData.find("|");
    str = strData.substr(0, pos2);
    pos1 = pos2 + 1;
    setAvpValue("Session-Id", str);

    pos2 = strData.find("|", pos1);
    str = strData.substr(pos1, pos2 - pos1);
    pos1 = pos2 + 1;
    sscanf(str.c_str(), "%d", &hh);


    pos2 = strData.find("|", pos1);
    str = strData.substr(pos1, pos2 - pos1);
    pos1 = pos2 + 1;
    sscanf(str.c_str(), "%d", &ee);

    setDiameterHeader(COMMAND_CODE_CC, true, 4, hh, ee);
    m_oMsg->hdr.flags.p = 1;


    pos2 = strData.find("|", pos1);
    str = strData.substr(pos1, pos2 - pos1);
    pos1 = pos2 + 1;
    setAvpValue("Destination-Realm", str);


    pos2 = strData.find("|", pos1);
    str = strData.substr(pos1, pos2 - pos1);
    pos1 = pos2 + 1;
    sscanf(str.c_str(), "%d", &uiTemp);
    setAvpValue("Auth-Application-Id", uiTemp);

    pos2 = strData.find("|", pos1);
    str = strData.substr(pos1, pos2 - pos1);
    pos1 = pos2 + 1;
    setAvpValue("Service-Context-Id", str);


    pos2 = strData.find("|", pos1);
    str = strData.substr(pos1, pos2 - pos1);
    pos1 = pos2 + 1;
    sscanf(str.c_str(), "%d", &iTemp);
    setAvpValue("CC-Request-Type", iTemp);



    pos2 = strData.find("|", pos1);
    str = strData.substr(pos1, strData.size() - pos1);
    sscanf(str.c_str(), "%d", &iTemp);
    setAvpValue("CC-Request-Number", iTemp);

    setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.NE-Host", m_strOriginHost);

    iTemp = 2;
    setAvpValue("Service-Information.Net-Ctrl-Information.NE-Information.NE-Type", iTemp);

    for(i = 0; i < iRows; i++)
    {
        sprintf(buf,
                "Service-Information.Net-Ctrl-Information.Service-Context-Information.Service-Context-Id[%d]",
                i);
        str = contextIDs[i];

        setAvpValue(buf, str);
    }

    int iRet = parseAppToRaw(str);
    if (iRet < 0)
    {
        return -1;
    }
    if ((unsigned int)iCCRSize < str.size())
    {
        return -1;
    }
    memcpy(sCCRbuf, str.c_str(), str.size());
    return CCR1;
}

/*
生成业务CCR包
	int createServCCR(char *sSrcBuf, int iSrcSize, char *sCCRbuf, int iCCRSize);
	sSrcBuf： 传入参数
		形式：<Session-Id>||{hop_给打包的ccr包by_hop}|{end_to_end}|{Auth-Application
-Id}|{CC-Request-Type}|{CC-Request-Number}|{Service-Context-Id}|{
application_id}|{Destination-Realm}^^^话单1^^^...^^^话单n
	iSrcSize：sSrcBuf的大小
	sCCRbuf： 打包后的CCR包
	iCCRSize：sCCRbuf的大小

*/
int OcpMsgParser::createServCCR(char *sSrcBuf, int iSrcSize, char *sCCRbuf, int iCCRSize)
{
    string strAvp;
    int ret = str2avp(CCR1, sSrcBuf, strAvp);
    if(ret < 0)
    {
        return -1;
    }
    if((unsigned int)iCCRSize < strAvp.size())
    {
        return -1;
    }
    memcpy(sCCRbuf, strAvp.c_str(), strAvp.size());
    return CCR1;
}

/*
生成业务CCA包
	int createServCCA(char *sSrcBuf, int iSrcSize, char *sCCAbuf, int CCASize);
	sSrcBuf：  传入参数
		形式：<Session-Id>|{hop_by_hop}|{end_to_end}|{Auth-Application-Id}|{CC-
Request-Type}|{CC-Request-Number}^^^Service-Result-Code|Para-Field-Result
^^^...^^^Service-Result-Code|Para-Field-Result
	iSrcSize：sSrcBuf的大小
	sCCAbuf： 打包后的CCA包
	CCASize： sCCAbuf的大小

*/
int OcpMsgParser::createServCCA(char *sSrcBuf, int iSrcSize, char *sCCAbuf, int CCASize)
{
    string strAvp;
    int ret = str2avp(CCA, sSrcBuf, strAvp);
    if(ret < 0)
    {
        return -1;
    }
    if((unsigned int)CCASize < strAvp.size())
    {
        return -1;
    }
    memcpy(sCCAbuf, strAvp.c_str(), strAvp.size());
    return CCA;
}

/*
int parserPack(char *sMsgBuf, int iMsgSize, char *sParseBuf, int iParseSize, char *sServiceContextID);
	// 在此函数中解析消息后，判断包类型是CCR、CCA或激活CCA时，将解析内容放到sParseBuf
	// 判断包类型是DWR或DPR时，生成对应的A包内容到sParseBuf
	// 判断包类型是CEA时，sParseBuf不填
	sMsgBuf   传入消息
	sParseBuf 返回解包内容 或 打包的A包内容
	CCR解包结构：<Session-Id>|{hop_by_hop}|{end_to_end}|{Auth-Application-Id}|{CC
-Request-Type}|{CC-Request-Number}|{Service-Context-Id}^^^话单1^^^话单2
^^^...^^^话单n
	CCA解包结构：<Session-Id>^^^Service-Result-Code|Para-Field-Result^^^...^^^
Service-Result-Code|Para-Field-Result
	激活CCA解包结构：Service-Result-Code
	sServiceContextID 返回Service-Context-Id（如果有的话）
	int返回值  返回原始消息的类型（如CCR、CCA等）

*/
int OcpMsgParser::parserPack(char *sMsgBuf, int iMsgSize, char *sParseBuf, int iParseSize, char *sServiceContextID)
{
    string strAvp;
    string strData;
    int ret;
    int iType = -1;
    string str;
    strAvp.assign(sMsgBuf, iMsgSize);
    ret = avp2str(strAvp, strData, iType);
    if (ret)
    {
        return -1;
    }
    if((unsigned int)iParseSize < strData.size())
    {
        return -1;
    }
    memcpy(sParseBuf, strData.c_str(), strData.size());
    switch(m_oMsg->hdr.code)
    {
    case COMMAND_CODE_CC:
    {
        getAvpValue("Service-Context-Id", str);
        strcpy(sServiceContextID, str.c_str());
        break;
    }
    case COMMAND_CODE_DW:
    case COMMAND_CODE_DP:
    case COMMAND_CODE_CE:
    {
        break;
    }
    default:
    {
        return -1;
    }
    }
    return iType;
}


// add by zhangch
// createDPR的参数   形式：hh|ee
int OcpMsgParser::createDWR(char *sArguments, char *sDWRbuf, int iDWRSize)
{
    releaseMsgAcl();
    string str;
    int hh = 0;
    int ee = 0;
    int pos1 = 0;
    int pos2 = 0;

    string strData = sArguments;

    pos2 = strData.find("|");
    str = strData.substr(0, pos2);
    pos1 = pos2 + 1;
    sscanf(str.c_str(), "%d", &hh);
    str = strData.substr(pos1);
    sscanf(str.c_str(), "%d", &ee);

    setDiameterHeader(COMMAND_CODE_DW, true, 0, hh, ee);

    setAvpValue("Origin-Host", m_strOriginHost);
    setAvpValue("Origin-Realm", m_strOriginRealm);

    int iRet = parseAppToRaw(str);
    if (iRet < 0)
    {
        return -1;
    }
    if ((unsigned int)iDWRSize < str.size())
    {
        return -1;
    }
    memcpy(sDWRbuf, str.c_str(), str.size());
    return DWR;
}

// add by zhangch
// createDPR的参数   形式：hh|ee
int OcpMsgParser::createDPR(char *sArguments, char *sDPRbuf, int iDPRSize)
{
    releaseMsgAcl();
    string str;
    int hh = 0;
    int ee = 0;
    int pos1 = 0;
    int pos2 = 0;

    string strData = sArguments;

    pos2 = strData.find("|");
    str = strData.substr(0, pos2);
    pos1 = pos2 + 1;
    sscanf(str.c_str(), "%d", &hh);
    str = strData.substr(pos1);
    sscanf(str.c_str(), "%d", &ee);

    setDiameterHeader(COMMAND_CODE_DP, true, 0, hh, ee);

    setAvpValue("Origin-Host", m_strOriginHost);
    setAvpValue("Origin-Realm", m_strOriginRealm);
    int cause = 0;
    setAvpValue("Disconnect-Cause", cause);

    int iRet = parseAppToRaw(str);
    if (iRet < 0)
    {
        return -1;
    }
    if ((unsigned int)iDPRSize < str.size())
    {
        return -1;
    }
    memcpy(sDPRbuf, str.c_str(), str.size());
    return DWR;

}

