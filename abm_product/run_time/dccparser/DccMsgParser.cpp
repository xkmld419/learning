#include <string>
#include <sstream>
#include <iostream>
#include "DccMsgParser.h"

#define GET_DATA_REF(dataType, data, containerEntryPtr) \
        dataType &data = (containerEntryPtr)->dataRef(Type2Type<dataType>())

/* template<class T>
std::string& to_string(std::string &result,const T& t)
{
 std::ostringstream oss;//创建一个流
 oss.clear();
 oss<<t;//把值传递如流中
 result=oss.str();//获取转换后的字符转并将其写入result
 return result;
}*/

static  char *request_scenario[4] = {"I", "U", "F", "E"};

void DccMsgParser::getServiceType(std::string &serviceContextId, char *serviceType)
{

  std::string scenario;
  std::string::size_type pos1, pos2;
  std::string tmp;
  strcpy(serviceType, "UNKNOWN");
  /*
  pos1 = serviceContextId.find(".", 0);
  pos2 = serviceContextId.find("@", 0);
  if( pos1 != std::string::npos && pos2 != std::string::npos )
  {
    if( pos2 > pos1 )
  	  tmp = serviceContextId.substr(pos1+1, pos2 - pos1 - 1);
    else
  	  tmp = serviceContextId.substr(0, pos2);

    pos1 = tmp.find(".", 0);
    if (pos1)
    	tmp = tmp.substr(0,pos1);
    	
  	strcpy(serviceType, tmp.c_str());
  }
  */
  pos1 = serviceContextId.find(".", 0);
  if (pos1 != std::string::npos)
  {
    tmp = serviceContextId.substr(0, pos1);
    strcpy(serviceType, tmp.c_str());
  }
}
char *ntp_fmt_ts(long reftime, char* buff)
{
	time_t temptime;
	struct tm *bd;
	memset(buff, 0, sizeof(buff));
  temptime = reftime - (long) NTP_BASETIME;
	bd = localtime(&temptime);
	if ( bd != NULL)
	{
		snprintf(buff, 100, "%04d%02d%02d%02d%02d%02d",
  		 bd->tm_year + 1900, bd->tm_mon + 1, bd->tm_mday,
			 bd->tm_hour, bd->tm_min, bd->tm_sec);
	}
	return buff;
}

void DccMsgParser::setDictHandle(bool output)
{
  ACE_INT32 type;

	if( getAvpValue("CC-Request-Type", type) <= 0 )
	  return ;

	if( type <= 0 || type > 4 )
	  return ;

  std::string serviceContextId;
	char serviceType[80];
  if( getAvpValue("Service-Context-Id", serviceContextId) <= 0 )
    return ;

	getServiceType(serviceContextId, serviceType);
//	printf("type=%d, Service-Context-Id=%s\n", type, serviceContextId.c_str());


  std::string  serviceIdentifier = std::string("");
  if( output)
  	  serviceIdentifier = "OUT";

//	printf("#################type=%d, Service-Context-Id=%s, serviceIdentifier=%s\n", type, serviceContextId.c_str(), serviceIdentifier.c_str());
  ::setDictHandle(*m_oMsg, request_scenario[type-1], serviceType, serviceIdentifier.c_str());

}

int DccMsgParser::getServiceInfo(char *buf, std::string &serviceContextId, std::string &serviceIdentifier)
{
	char tmp_buf[80];
	serviceContextId = std::string("");
	serviceIdentifier = std::string("");
	if( strncmp(buf, "55AA", 4) == 0 )
	{
		memcpy(tmp_buf, &buf[4], 4);
		tmp_buf[4] = 0 ;
		int len = atoi(tmp_buf);
		memcpy(tmp_buf, &buf[8], len-8);
		char *p1, *p2, tmp_buf1[80];
		if( tmp_buf[0] == '|' )
		{
			if( (p1 = (char *)strstr(&tmp_buf[1], "|")) != NULL)
			{
        memset(tmp_buf1, 0, 80 );
        memcpy(tmp_buf1, &tmp_buf[1], p1 - tmp_buf - 1 ) ;
			  serviceContextId = std::string(tmp_buf1);
			  if( (p2 = (char *)strstr(p1+1, "|")) != NULL)
			  {
          memset(tmp_buf1, 0, 80 );
          memcpy(tmp_buf1, p1+1, p2 - p1 - 1 ) ;
		      serviceIdentifier = std::string(tmp_buf1);
			  }
			}
		}
	  return len;
	}
	else
	  return 0;
}

int DccMsgParser::parseRawToApp( void *buf, int bufSize, ParseOption option)
{
		AAAMessageBlock *aBuffer;
  	HeaderParser hp;

		m_oMsg->acl.releaseContainers();
		setSetFlag(false);
		aBuffer = AAAMessageBlock::Acquire((char*)buf, bufSize);
  	hp.setRawData(aBuffer);
  	hp.setAppData(&m_oMsg->hdr);
  	hp.setDictData(option);
  	try {
    		hp.parseRawToApp();
  	}
  	catch (AAAErrorStatus st)
    	{
      		std::cout << "header error" << std::endl;
      		int type, code;
      		std::string avp;
      		st.get(type, code, avp);
      		aBuffer->Release();
      		return -code;
    	}

#ifdef DEBUG_DISP
	  printf("m_oMsg->hdr.code=%d, option=%d,msg.hdr.length=%d\n", m_oMsg->hdr.code, option, m_oMsg->hdr.length);
#endif

    ::setRequestFlag(m_oMsg->hdr.flags.r);
  	PayloadParser pp;
  	aBuffer->size(m_oMsg->hdr.length);
  	pp.setRawData(aBuffer);
  	pp.setAppData(&m_oMsg->acl);
  	pp.setDictData(m_oMsg->hdr.getDictHandle());
		if( m_oMsg->hdr.code == COMMAND_CODE_CC && option == PARSE_STRICT )
	  	pp.setListNum(1);
		else
	  	pp.setListNum(4);
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
      		m_sFailedAvp = avp;
      		std::cout << "Error type=" << type << ", code="
					<< code << ", name=" << avp << std::endl;
//     		m_oMsg->acl.releaseContainers();
      		aBuffer->Release();
      		return -code;
  	}

//	printf("2.msg.hdr.length=%d\n", m_oMsg->hdr.length);

	if( m_oMsg->hdr.code == COMMAND_CODE_CC && option == PARSE_STRICT )
	{

    this->setDictHandle();
		aBuffer->rd_ptr(aBuffer->base() + HEADER_SIZE);
		aBuffer->size(m_oMsg->hdr.length);
		pp.setRawData(aBuffer);
		pp.setAppData(&m_oMsg->acl);

		pp.setDictData(m_oMsg->hdr.getDictHandle());
	  pp.setListNum(3);
		try {
    			pp.parseRawToApp();
  	}
  	catch (AAAErrorStatus st) {
  			std::cout << "2.payload parser error" << std::endl;
  			int type, code;
   			std::string avp;
   			st.get(type, code, avp);
     		m_sFailedAvp = avp;
   			std::cout << "Error type=" << type << ", code="
			    << code << ", name=" << avp << std::endl;
//    		m_oMsg->acl.releaseContainers();
   			aBuffer->Release();
     		return -code;
  	}

	}
	aBuffer->Release();
// printf("parseRawToApp ok\n");
	return 0;
}

int DccMsgParser::parseRawToApp( std::string& str_buf, ParseOption option)
{
	return parseRawToApp((unsigned char *) str_buf.data(), str_buf.length(), option);
}



void DccMsgParser::printHeader()
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
}

int DccMsgParser::parseAppToRaw( void *buf, int size, bool output, bool bad_answer, ParseOption option  )
{
  	AAAMessageBlock *aBuffer;
  	HeaderParser hp;
		setSetFlag(false);
  	aBuffer = AAAMessageBlock::Acquire((char*)buf, size);
  	hp.setRawData(aBuffer);
  	hp.setAppData(&m_oMsg->hdr);
  	hp.setDictData(option);

  	/* Check validity of flags and gets a AAACommand structure */
		try {
			hp.parseAppToRaw();
		}
		catch (AAAErrorStatus st)
		{
			std::cout << "header error" << std::endl;;
			aBuffer->Release();
			return -1;
    }

    ACE_INT32 int32=AAA_SUCCESS;
    getAvpValue("Result-Code", int32);
    ::setResultCode(m_oMsg->hdr.flags.r, int32);

	  if( m_oMsg->hdr.code == COMMAND_CODE_CC && option == PARSE_STRICT)
	  {
  	  if( bad_answer)
  	  {
  	    ::setDictHandle(*m_oMsg, "", "", "ERROR");
      }
      else
        this->setDictHandle(output);
    }

    if( output )
    {
		  if( (m_oMsg->hdr.code == COMMAND_CODE_RA || m_oMsg->hdr.code == COMMAND_CODE_AS ) && option == PARSE_STRICT )
      {
    	  ::setDictHandle(*m_oMsg, "", "", "OUT");
      }
    }

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
			std::cout << "assemble failed" << std::endl;
      getAllAvp();
			m_oMsg->acl.releaseContainers();
			aBuffer->Release();
			return -1;
		}

		m_oMsg->hdr.length = aBuffer->wr_ptr() - aBuffer->base();

	/* The second calll of hp.set() sets the actual message length */
		try {
			hp.parseAppToRaw();
		}
		catch (AAAErrorStatus st)
		{
			std::cout << "header error" << std::endl;
			m_oMsg->acl.releaseContainers();
			aBuffer->Release();
			return -1;
		}

//m_oMsg->acl.releaseContainers();
		aBuffer->Release();
		return (m_oMsg->hdr.length);
};

int DccMsgParser::parseAppToRaw( std::string& str_buf, bool output, ParseOption option)
{
	//unsigned char buf[8192];
	unsigned char buf[81920];
	int len;
	if( (len = parseAppToRaw( buf, 81920, output, false, option )) > 0)
	{
		str_buf.assign((char *)buf, len);
		return len;
	}
	return -1;
}

int DccMsgParser::deleteAvpValue(char *avp_name1)
{
   AAAAvpContainer* c;
   int quit_flag = 0;
   std::string::size_type pos, start_pos ;
   std::string avpName, avp_name = std::string(avp_name1);
   std::string::size_type pos1, pos2, pos3;
   AAAAvpContainerList* acl = &m_oMsg->acl;
	for( pos = 0, start_pos = 0; !quit_flag ;  )
  {
   	pos1 = avp_name.find(".", start_pos);
		if( pos1 != std::string::npos )
		{
			avpName =  avp_name.substr(start_pos, pos1-start_pos);
			pos2 = avpName.find("[", 0);
			if( pos2 != std::string::npos)
			{
				pos3 = avpName.find("]", pos2);
				if( pos3 != std::string::npos)
					pos = atoi(avpName.substr(pos2+1, pos3-pos2-1).c_str());
				avpName.erase(pos2, 10);
			}
			start_pos = pos1+1;
		}
		else
		{
		  pos2 = avp_name.find("[", start_pos);
		  if( pos2 != std::string::npos )
		  {
		    pos3 = avp_name.find("]", pos2);
			  if( pos3 != std::string::npos)
			    pos = atoi(avp_name.substr(pos2+1, pos3-pos2-1).c_str());
    	  avpName = avp_name.substr(start_pos, pos2-start_pos);;
		  }
		  else
      {
        avpName = avp_name.substr(start_pos, 500);
      }
			quit_flag = 1 ;
		}

//	printf("avpName:[%s]\n", avpName.c_str());
  	if( (c = acl->search(avpName.c_str())) )
  	{
			if( c->size() > pos )
			{
				if( ((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE )
				{
					acl = ((*c)[pos])->dataPtr(Type2Type<AAAAvpContainerList>());
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
				return -1;
			}
   	}
   	else
		{
#ifdef DEBUG_DISP
			printf("avpName[%s] not found\n", avpName.c_str());
#endif
   		return -1;
  	}
  }
  return 0;

}


template<class T>
int DccMsgParser::_getAvpValue(char *avp_name1, T &data)
{
   AAAAvpContainer* c;
   int quit_flag = 0;
   std::string::size_type pos, start_pos ;
   std::string avpName, avp_name = std::string(avp_name1);
   std::string::size_type pos1, pos2, pos3;
   AAAAvpContainerList* acl = &m_oMsg->acl;
	for( pos = 0, start_pos = 0; !quit_flag ;  )
  {
   	pos1 = avp_name.find(".", start_pos);
		if( pos1 != std::string::npos )
		{
			avpName =  avp_name.substr(start_pos, pos1-start_pos);
			pos2 = avpName.find("[", 0);
			if( pos2 != std::string::npos)
			{
				pos3 = avpName.find("]", pos2);
				if( pos3 != std::string::npos)
					pos = atoi(avpName.substr(pos2+1, pos3-pos2-1).c_str());
				avpName.erase(pos2, 10);
			}
			start_pos = pos1+1;
		}
		else
		{
		  pos2 = avp_name.find("[", start_pos);
		  if( pos2 != std::string::npos )
		  {
		    pos3 = avp_name.find("]", pos2);
			  if( pos3 != std::string::npos)
			    pos = atoi(avp_name.substr(pos2+1, pos3-pos2-1).c_str());
    	  avpName = avp_name.substr(start_pos, pos2-start_pos);;
		  }
		  else
      {
        avpName = avp_name.substr(start_pos, 500);
      }
			quit_flag = 1 ;
		}

//	printf("avpName:[%s]\n", avpName.c_str());
  	if( (c = acl->search(avpName.c_str())) )
  	{
			if( c->size() > pos )
			{
				if( ((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE )
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
				return -1;
			}
   	}
   	else
		{
#ifdef DEBUG_DISP
			printf("avpName[%s] not found\n", avpName.c_str());
#endif
   		return -1;
  	}
  }
  return 0;

}

template<class T>
int  DccMsgParser::_setAvpValue( char *avp_name1, T &data, AAA_AVPDataType t)
{
  AAAAvpContainerManager cm;
  AAAAvpContainerEntryManager em;
  AAAAvpContainerEntry *e;
  int quit_flag = 0, ret = 0, flag, set_flag ;
  std::string::size_type pos, start_pos;
  std::string avpName, avp_name = std::string(avp_name1);
  std::string::size_type pos1, pos2, pos3;
  AAAAvpContainerList *acl = &m_oMsg->acl;
  AAAAvpContainerList *tmp_list = NULL;
  AAAAvpContainer* c;
  for( pos = 0, start_pos = 0, flag = 0, set_flag = 0; !quit_flag ;  )
  {
   	pos1 = avp_name.find(".", start_pos);
	  if( pos1 != std::string::npos )
	  {
		  avpName =  avp_name.substr(start_pos, pos1-start_pos);
		  pos2 = avpName.find("[", 0);
		  if( pos2 != std::string::npos)
		  {
			  pos3 = avpName.find("]", pos2);
			  if( pos3 != std::string::npos)
				  pos = atoi(avpName.substr(pos2+1, pos3-pos2-1).c_str());
			  avpName.erase(pos2, 10);

		  }

		  start_pos = pos1+1;
    }
    else
    {
    	pos2 = avp_name.find("[", start_pos);
    	if( pos2 != std::string::npos )
    	{
    	  pos3 = avp_name.find("]", pos2);
    		if( pos3 != std::string::npos)
    		  pos = atoi(avp_name.substr(pos2+1, pos3-pos2-1).c_str());
      	avpName = avp_name.substr(start_pos, pos2-start_pos);;
    	}
    	else
      {
        avpName = avp_name.substr(start_pos, 500);
      }
		  quit_flag = 1 ;
	  }
//	printf("avpName:[%s]\n", avpName.c_str());
    if( !flag && (c = acl->search(avpName.c_str()) ) )
    {
//		printf("1.avpName:[%s]\n", avpName.c_str());
		  if( c->size() > pos )
		  {
			  if( ((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE )
			  {
				  acl = ((*c)[pos])->dataPtr(Type2Type<AAAAvpContainerList>());
				  pos = 0;
				  set_flag = 1 ;
//				printf("2.avpName:[%s],set_flag=1\n", avpName.c_str());
			  }
			  else
			  {
//				printf("3.avpName:[%s],set_value\n", avpName.c_str());
			    if( t != AAA_AVP_DATA_TYPE )
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
	      if( !quit_flag )
			  {
//				printf("11.avpName:[%s],set_flag\n", avpName.c_str());
				  e = em.acquire(AAA_AVP_GROUPED_TYPE);
				  acl = e->dataPtr(Type2Type<diameter_grouped_t>());
    	      			c->add(e);

   		  }
			  else
			  {
			    if( t != AAA_AVP_DATA_TYPE )
				  {
				    e = em.acquire(t);
            GET_DATA_REF(T, tmp, e);
      	    c->add(e);
            tmp = data;
				  }
				  ret = 1;
			  }
			  flag = 1 ;
		  }
   	}
   	else
   	{


//		printf("2.avpName:[%s]\n", avpName.c_str());
     	c = cm.acquire(avpName.c_str());
      if( !quit_flag )
      {
//			printf("5.avpName:[%s],set_flag\n", avpName.c_str());
			  e = em.acquire(AAA_AVP_GROUPED_TYPE);
			  tmp_list = e->dataPtr(Type2Type<diameter_grouped_t>());
    	  c->add(e);
     	}
      else
      {
//			printf("4.avpName:[%s],set_value\n", avpName.c_str());
//			cout << "data:t" << t << ":" << data << endl;
        if( t != AAA_AVP_DATA_TYPE )
				{
			    e = em.acquire(t);
			    GET_DATA_REF(T, tmp, e);
      	  c->add(e);
     	    tmp = data;
//			cout << "data:" << data << ":" << tmp << endl;
			  }
			  ret = 1;
      }

      if( acl )
      {
//	  	printf("2.acl add\n");
	    	acl->add(c);
   		}
		  acl = tmp_list;
  		flag = 1 ;
    }

  }
  if( ret)
	  setSetFlag(true);
  return ret;
}

int DccMsgParser::getDiameterCommandCode()
{
	if( m_oMsg->hdr.flags.r  == AAA_FLG_SET )
	{
    switch( m_oMsg->hdr.code )
    {
			case 	COMMAND_CODE_CC:
				return CCR1;
			case 	COMMAND_CODE_RA:
				return RAR;
			case	COMMAND_CODE_AS:
				return ASR;
			case	COMMAND_CODE_DW:
				return DWR;
			case	COMMAND_CODE_DP:
				return DPR;
			case	COMMAND_CODE_ST:
				return STR;
			case	COMMAND_CODE_CE:
				return CER;
			default:
				return NOKNOWN;
		}
	}
	else
	{
    switch( m_oMsg->hdr.code )
    {
			case 	COMMAND_CODE_CC:
				return CCA;
			case 	COMMAND_CODE_RA:
				return RAA;
			case	COMMAND_CODE_AS:
				return ASA;
			case	COMMAND_CODE_DW:
				return DWA;
			case	COMMAND_CODE_DP:
				return DPA;
			case	COMMAND_CODE_ST:
				return STA;
			case	COMMAND_CODE_CE:
				return CEA;
			default:
				return NOKNOWN;
		}
	}
}


int  DccMsgParser::setAvpValue( char *avp_name, int &data)
{
 	ACE_INT32 tmp_data = static_cast<ACE_INT32> (data);
 	return _setAvpValue( avp_name, tmp_data, AAA_AVP_INTEGER32_TYPE);
}

int  DccMsgParser::setAvpValue( char *avp_name, ACE_UINT32 &data)
{
 	ACE_UINT32 tmp_data = static_cast<ACE_UINT32> (data);
 	return _setAvpValue( avp_name, tmp_data, AAA_AVP_UINTEGER32_TYPE);
}

int  DccMsgParser::setAvpValue( char *avp_name, long &data)
{
 	ACE_INT32 tmp_data = static_cast<ACE_INT32> (data);
 	return _setAvpValue( avp_name, tmp_data, AAA_AVP_INTEGER32_TYPE);
}

int  DccMsgParser::setAvpValue( char *avp_name, ACE_UINT64 &data)
{
	return _setAvpValue( avp_name, data, AAA_AVP_UINTEGER64_TYPE);
	/*
	#ifdef DEF_LINUX
	  ACE_UINT64 tmp = ntohq(data) ;
	 	return _setAvpValue( avp_name, tmp, AAA_AVP_UINTEGER64_TYPE);
 	#else
 	  return _setAvpValue( avp_name, data, AAA_AVP_UINTEGER64_TYPE);
 	#endif
 	*/
}

int  DccMsgParser::setAvpValue( char *avp_name, diameter_address_t &data)
{
 	return _setAvpValue( avp_name, data, AAA_AVP_ADDRESS_TYPE);
}


int  DccMsgParser::setAvpValue( char *avp_name, std::string &data)
{
 	return _setAvpValue( avp_name, data, AAA_AVP_STRING_TYPE);
}

int  DccMsgParser::setAvpValue( char *avp_name)
{
	std::string data;
	char tmp_name[200];
	sprintf(tmp_name, "%s.AVP", avp_name);
 	return _setAvpValue( tmp_name, data, AAA_AVP_DATA_TYPE);
}


int  DccMsgParser::setAvpValue( char *avp_name, float &data)
{
	ACE_UINT64 unit_digits;
	ACE_INT32  exponent;
//	int 	unit_digits;
//	int	exponent;
	float data1 = data;
	int i;

	char tmp_avp_name[300], *p;
	sprintf(tmp_avp_name, "%f", data);
//	printf("tmp_avp_name=%s\n", tmp_avp_name);
	if( (p = (char *)strstr(tmp_avp_name, ".")))
	{
		exponent = strlen(p+1);
	 	for ( i = 0 ; i < exponent ; i ++ )
 			data1 *= 10;
		unit_digits = (ACE_UINT64) data1;
//		unit_digits = (int) data1;
#ifdef DEBUG_DISP
		std::cout << "p=" << p + 1 << ":1.unit_digits=" << unit_digits << ",exponent=" << exponent << std::endl;
#endif
	}
	else
	{
		unit_digits = (ACE_UINT64) data;
//		unit_digits = (int) data;
		exponent = 0;
#ifdef DEBUG_DISP
		std::cout << "2.unit_digits=" << unit_digits << ",exponent=" << exponent << std::endl;
#endif
	}

//	std::cout << "3.unit_digits=" << unit_digits << ",exponent=" << exponent << std::endl;
	sprintf( tmp_avp_name, "%s.Value-Digits", avp_name);
 	if( _setAvpValue( tmp_avp_name, unit_digits, AAA_AVP_INTEGER64_TYPE) == 0 )
 		return 0;
	if( exponent == 0 )
		return 1;
	sprintf( tmp_avp_name, "%s.Exponent", avp_name);
 	if( _setAvpValue( tmp_avp_name, exponent, AAA_AVP_INTEGER32_TYPE) == 0 )
 		return 0;
	return 1;
}



int  DccMsgParser::getAvpValue( char *avp_name, int &data)
{
	ACE_INT32 tmp_data ;
 	int ret = _getAvpValue( avp_name, tmp_data);
 	if( ret > 0 )
 		data = static_cast<int> (tmp_data);
 	return ret;
}

int  DccMsgParser::getAvpValue( char *avp_name, long &data)
{
	ACE_INT32 tmp_data;
 	int ret = _getAvpValue( avp_name, tmp_data);
 	if( ret > 0 )
 		data = static_cast<long> (tmp_data);
 	return ret;
}

int  DccMsgParser::getAvpValue( char *avp_name, ACE_UINT32 &data)
{
	ACE_UINT32 tmp_data;
 	int ret = _getAvpValue( avp_name, tmp_data);
 	if( ret > 0 )
 		data = static_cast<long> (tmp_data);
 	return ret;
}


int  DccMsgParser::getAvpValue( char *avp_name, ACE_UINT64 &data)
{
 	return _getAvpValue( avp_name, data);
}


int  DccMsgParser::getAvpValue( char *avp_name, std::string &data)
{
	data.clear();
 	return _getAvpValue( avp_name, data);
}

int  DccMsgParser::getAvpValue( char *avp_name, diameter_address_t &data)
{
 	return _getAvpValue( avp_name, data);
}

int  DccMsgParser::getAvpValue( char *avp_name, float &data)
{
	ACE_UINT64 unit_digits;
	ACE_INT32 exponent;
	int i;
	char tmp_avp_name[300];
	sprintf( tmp_avp_name, "%s.Value-Digits", avp_name);
 	if( _getAvpValue( tmp_avp_name, unit_digits) == 0 )
 		return 0;
	sprintf( tmp_avp_name, "%s.Exponent", avp_name);
 	if( _getAvpValue( tmp_avp_name, exponent) == 0 )
	{
	 	data = (float)unit_digits;
 		return 0;
 	}
 	data = (float)unit_digits;
 	for ( i = 0 ; i < exponent ; i ++ )
 		data /= 10;
// 	printf("data=%f\n", data);
	return 1;
}


int DccMsgParser::getGroupedAvpNums1(const char *avp_name1)
{
	AAAAvpContainer* c;
	char tmp_name[300], avp_name[300], *p;
	int i , flag ;
	AAAAvpContainerList *acl = &m_oMsg->acl;
	strcpy(avp_name, avp_name1);
	for( i = 0, flag = 0; ; i ++  )
	{
		if( (p = (char *) strstr(avp_name, ".")) )
		{
    	memset(tmp_name, 0, 200);
    	memcpy(tmp_name, avp_name, p - avp_name);
      strcpy(avp_name, tmp_name);
      if( (c = acl->search(tmp_name)) )
			{
 				if( ((*c)[0]->dataType()) == AAA_AVP_GROUPED_TYPE )
				{
					acl = ((*c)[0])->dataPtr(Type2Type<AAAAvpContainerList>());
					strcpy(tmp_name, p+1);
					strcpy(avp_name, tmp_name);
				}
			}
		}
		else
   	{
      if( (c = acl->search(avp_name)) )
			{
				if( ((*c)[0]->dataType()) == AAA_AVP_GROUPED_TYPE )
				{
					return c->size();
				}
			}
			break;
    }
  }
  return 0;
}

int DccMsgParser::getGroupedAvpNums(const char *avp_name1)
{
   AAAAvpContainer* c;
   int quit_flag = 0;
   std::string::size_type pos, start_pos ;
   std::string avpName, avp_name = std::string(avp_name1);
   std::string::size_type pos1, pos2, pos3;
   AAAAvpContainerList* acl = &m_oMsg->acl;
	for( pos = 0, start_pos = 0; !quit_flag ;  )
  {
   	pos1 = avp_name.find(".", start_pos);
		if( pos1 != std::string::npos )
		{
			avpName =  avp_name.substr(start_pos, pos1-start_pos);
			pos2 = avpName.find("[", 0);
			if( pos2 != std::string::npos)
			{
				pos3 = avpName.find("]", pos2);
				if( pos3 != std::string::npos)
					pos = atoi(avpName.substr(pos2+1, pos3-pos2-1).c_str());
				avpName.erase(pos2, 10);
			}
			start_pos = pos1+1;
		}
		else
		{
			avpName = avp_name.substr(start_pos, 500);;
			quit_flag = 1 ;
		}

//	printf("avpName:[%s]\n", avpName.c_str());
  	if( (c = acl->search(avpName.c_str())) )
  	{
			if( c->size() > pos )
			{
				if( ((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE )
				{
					acl = ((*c)[pos])->dataPtr(Type2Type<AAAAvpContainerList>());
					pos = 0;
					if( quit_flag )
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
				return -1;
			}
   	}
   	else
		{

#ifdef DEBUG_DISP
			printf("avpName[%s] not found\n", avpName.c_str());
#endif

   		return -1;
  	}
  }
  return 0;

}


void DccMsgParser::getAvpName(std::vector<std::string>& sAvpName, std::string& avp_name)
{
	std::vector<std::string>::iterator itor;

	for (itor = sAvpName.begin(); itor != sAvpName.end(); itor ++ )
	{
		if( itor == sAvpName.begin())
			avp_name = (*itor);
		else
			avp_name += "." + (*itor) ;

	}
}

void DccMsgParser::setOutStr(std::string& avp_name, char *sFormatStr, const char *sTmpBuf)
{
	if( avp_name.find("LCOCS", 0) == std::string::npos )
	  m_sAvpOutStr += avp_name + sFormatStr + sTmpBuf + "\n";
}


void DccMsgParser::getAvpData(AAAAvpContainerEntry *e, int pos, int size, std::vector<std::string>& sAvpName)
{
	std::string tmp;
	std::string avp_name="";
	char sTmpBuf[512],sFormatStr[80];
	std::string sTmpStr; 
	int i;
	if( size > 1 )
	  sprintf(sFormatStr, "[%d]:", pos);
  else
	  strcpy(sFormatStr, ":");
	switch( e->dataType())
	{
		  case AAA_AVP_INTEGER32_TYPE:
		  	{
//		  		to_string(tmp, e->dataRef(Type2Type<diameter_integer32_t>()) );
		  		  sprintf(sTmpBuf, "%ld", e->dataRef(Type2Type<diameter_integer32_t>()));
					  getAvpName(sAvpName, avp_name);
		  		  if( m_bDispFlag )
		  			  std::cout << avp_name << sFormatStr << std::string(sTmpBuf) << std::endl;
//					  mapInt32[avp_name] = e->dataRef(Type2Type<diameter_integer32_t>());
					  setOutStr(avp_name, sFormatStr, sTmpBuf);
	   		}
			break;
		  case AAA_AVP_UINTEGER32_TYPE:
    			{
//		  		to_string(tmp, e->dataRef(Type2Type<diameter_unsigned32_t>()) );
		  			sprintf(sTmpBuf, "%lu", e->dataRef(Type2Type<diameter_unsigned32_t>()));
						getAvpName(sAvpName, avp_name);
		  			if( m_bDispFlag )
		  				std::cout << avp_name << sFormatStr << std::string(sTmpBuf) << std::endl;
//						mapInt32[avp_name] = e->dataRef(Type2Type<diameter_unsigned32_t>());
					  setOutStr(avp_name, sFormatStr, sTmpBuf);
    			}
			break;

		  case AAA_AVP_INTEGER64_TYPE:
    			{
//		  			to_string(tmp, e->dataRef(Type2Type<diameter_integer64_t>()) );
						#if defined(OS_WIN32)
						sprintf(sTmpBuf, "%I64d", e->dataRef(Type2Type<diameter_integer64_t>()));
						#elif defined(OS_LINUX) || defined(OS_CYGWIN)
						sprintf(sTmpBuf, "%lld", e->dataRef(Type2Type<diameter_integer64_t>()));
						#else
						sprintf(sTmpBuf, "%ld", e->dataRef(Type2Type<diameter_integer64_t>()));
						#endif
						getAvpName(sAvpName, avp_name);
		  			if( m_bDispFlag )
		  				std::cout << avp_name << sFormatStr << sTmpBuf << std::endl;
					  setOutStr(avp_name, sFormatStr, sTmpBuf);
//						mapInt64[avp_name] = e->dataRef(Type2Type<diameter_integer64_t>());
    			}
			break;
		  case AAA_AVP_UINTEGER64_TYPE:
    			{
	//	  		to_string(tmp, e->dataRef(Type2Type<diameter_unsigned64_t>()) );
						#if defined(OS_WIN32)
						sprintf(sTmpBuf, "%I64u", e->dataRef(Type2Type<diameter_unsigned64_t>()));
						#elif defined(OS_LINUX) || defined(OS_CYGWIN)
						sprintf(sTmpBuf, "%llu", e->dataRef(Type2Type<diameter_unsigned64_t>()));
						#else
						sprintf(sTmpBuf, "%lu", e->dataRef(Type2Type<diameter_unsigned64_t>()));
						#endif
						getAvpName(sAvpName, avp_name);
		  			if( m_bDispFlag )
		  				std::cout << avp_name << sFormatStr << sTmpBuf << std::endl;
					  setOutStr(avp_name, sFormatStr, sTmpBuf);
//						mapInt64[avp_name] = e->dataRef(Type2Type<diameter_unsigned64_t>());
    			}
			break;
		  case AAA_AVP_ENUM_TYPE:
    			{
		  			// to_string(tmp, e->dataRef(Type2Type<diameter_integer32_t>()) );
		  			sprintf(sTmpBuf, "%d", e->dataRef(Type2Type<diameter_integer32_t>()));
						getAvpName(sAvpName, avp_name);
		  			if( m_bDispFlag )
		  				std::cout << avp_name << sFormatStr << std::string(sTmpBuf) << std::endl;
//						mapInt32[avp_name] = e->dataRef(Type2Type<diameter_integer32_t>());
					  setOutStr(avp_name, sFormatStr, sTmpBuf);
    			}
    			break;
		  case AAA_AVP_STRING_TYPE:
    			{
						getAvpName(sAvpName, avp_name);
				  //  strcpy(sTmpBuf, e->dataRef(Type2Type<diameter_octetstring_t>()).c_str());
				    sTmpStr = e->dataRef(Type2Type<diameter_octetstring_t>());
		  			if( avp_name.find("LCOCS-PayAcctItemList",0) != std::string::npos ||
		  				  avp_name.find("LCOCS-HoldAcctItemList",0) != std::string::npos )
						{
		  			  tmp = e->dataRef(Type2Type<diameter_octetstring_t>());
			  			if( m_bDispFlag )
			  			{
			  				std::cout << avp_name << sFormatStr << "length:[" << tmp.size()<< "]" << std::endl;
						  }
						}
						else
						{
							int i;
			  			if( m_bDispFlag )
			  			{
			  			  tmp = e->dataRef(Type2Type<diameter_octetstring_t>());
			  			  //strcpy(sTmpBuf, tmp1.c_str());
			  			  std::cout << avp_name << sFormatStr ;
			  			  for( i = 0 ; i < tmp.size(); i ++ )
			  			  {
			  			  	if( isprint(tmp[i]))
			  			  	  printf("%c", tmp[i]);
			  				  else
			  				    printf("\\x%02X", tmp[i]&0xFF);
			  			  }
			  			  printf("\n");
			  			}
		  				  //   << sFormatStr <<  << std::endl;
						}
					  setOutStr(avp_name, sFormatStr, sTmpStr.c_str());
//						mapStr[avp_name] = e->dataRef(Type2Type<diameter_octetstring_t>());
    			}
			    break;

		  case AAA_AVP_ADDRESS_TYPE:
    			{
						getAvpName(sAvpName, avp_name);
						diameter_address_t &addr = e->dataRef(Type2Type<diameter_address_t>());
						sprintf(sTmpBuf, "%d:", addr.type);
						int len =  addr.value.length();
						unsigned char tmp[100];
						memcpy(tmp, addr.value.c_str(), len ) ;
						for( i = 0 ; i < len ; i ++ )
							sprintf(sTmpBuf+strlen(sTmpBuf), "%d.", tmp[i]);

						if( sTmpBuf[strlen(sTmpBuf)-1] == '.')
								sTmpBuf[strlen(sTmpBuf)-1] = 0 ;

		  			if( m_bDispFlag )
			  			std::cout << avp_name << sFormatStr << sTmpBuf << std::endl;
					  setOutStr(avp_name, sFormatStr, sTmpBuf);
    			}
			    break;
		  case AAA_AVP_UTF8_STRING_TYPE:
    			{
				    getAvpName(sAvpName, avp_name);
						//strcpy( sTmpBuf, e->dataRef(Type2Type<diameter_utf8string_t>()).c_str());
						sTmpStr = e->dataRef(Type2Type<diameter_utf8string_t>());
						
		  		  if( m_bDispFlag )
			  		  std::cout << avp_name << sFormatStr << e->dataRef(Type2Type<diameter_utf8string_t>()) << std::endl;
//			    mapStr[avp_name] = e->dataRef(Type2Type<diameter_octetstring_t>());
					 // setOutStr(avp_name, sFormatStr, sTmpBuf);
					 setOutStr(avp_name, sFormatStr, sTmpStr.c_str());
    			}
			break;
		  case AAA_AVP_DIAMID_TYPE:
    			{
				    getAvpName(sAvpName, avp_name);
						strcpy( sTmpBuf, e->dataRef(Type2Type<diameter_identity_t>()).c_str());
		  		  if( m_bDispFlag )
			  		  std::cout << avp_name << sFormatStr << e->dataRef(Type2Type<diameter_identity_t>()) << std::endl;
//				    mapStr[avp_name] = e->dataRef(Type2Type<diameter_octetstring_t>());
					  setOutStr(avp_name, sFormatStr, sTmpBuf);
    			}
			    break;
	  	case AAA_AVP_DATA_TYPE:
		  case AAA_AVP_DIAMURI_TYPE:
		  case AAA_AVP_TIME_TYPE:
    		  case AAA_AVP_GROUPED_TYPE:
    		  case AAA_AVP_IPFILTER_RULE_TYPE:
		  case AAA_AVP_CUSTOM_TYPE:
			break;
	}

}


void DccMsgParser::_getAllAvp(AAAAvpContainerList *orig_acl, std::vector<std::string>& sAvpName)
{
  AAAAvpContainer *c;
  AAAAvpContainerList *acl = orig_acl, *acl1;
  AAAAvpContainerList::iterator itor;
  unsigned int i;
  std::string tmp;
  char tmp_buf[20];
  for (itor = acl->begin(); itor != acl->end(); itor ++ )
  {

  	c = *itor;
	  for( i = 0 ; i < c->size() ; i ++ )
	  {

		  if( ((*c)[i]->dataType()) == AAA_AVP_GROUPED_TYPE )
  	  {
			  if( c->size() > 1 )
			  {
				  sprintf(tmp_buf, "%d", i );
				  tmp = c->getAvpName() + std::string("[") + std::string(tmp_buf) + std::string("]");
				  sAvpName.push_back(tmp);
			  }
			  else
			  {
          if( strstr(c->getAvpName(), "Requested-Service-Unit") &&
    			   ( sAvpName.size() > 0 &&  strstr(sAvpName.back().c_str(), "Multiple-Services-Credit-Control")) )
    			{
    			  std::string avp_name="";
				    sAvpName.push_back(c->getAvpName());
            getAvpName(sAvpName, avp_name);
				    sAvpName.pop_back();
		        if( m_bDispFlag )
				      std::cout << avp_name << ":" << std::endl;
			      m_sAvpOutStr += avp_name + ":" + "\n";
			    }
				  sAvpName.push_back(c->getAvpName());
			  }
			  acl1 = ((*c)[i])->dataPtr(Type2Type<AAAAvpContainerList>());
  			_getAllAvp(acl1, sAvpName);
  			sAvpName.pop_back();
  		}
  		else
  		{
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

std::string& DccMsgParser::getAllAvp(bool disp_flag)
{
  AAAAvpContainerList *acl = &m_oMsg->acl;
  std::vector<std::string> sAvpName;
  m_sAvpOutStr = std::string("");
  m_bDispFlag  = disp_flag;
  if(m_bDispFlag)
     printHeader();

  _getAllAvp(acl, sAvpName);
  return m_sAvpOutStr;
}

int DccMsgParser::parseAppToRawWithCommandFail(int code, unsigned char *buf)
{
  unsigned char *p;
  int msg_len;
  buf[1] = 0 ;
  buf[2] = 0 ;
  buf[3] = 0x20;
  buf[4] &= 0x7F;
  memcpy(&buf[20], "\x00\x00\x01\x0C\x40\x00\x00\x0C", 8 ) ;
  p = &buf[28];

//  *((ACE_UINT32*)(p)) = ntohl(code);
  ACE_INT32 int32 = ntohl(code);
  memcpy(p, &int32, sizeof(ACE_INT32));
  msg_len = 32;
  return msg_len;
//  disp_buf(buf, 32);
}

void DccMsgParser::setAvpCheck(int  avp_check)
{
  ::setAvpCheck(avp_check);
}


//  性能用
int  DccMsgParser::setMsgBuf(std::string &msg_buf)
{
  memcpy(m_sMsgBuf, (char *)msg_buf.data(), msg_buf.length());
  m_iMsgLen = ntohl(*((ACE_UINT32*)(m_sMsgBuf)))& 0x00ffffff;
  m_oMsg->hdr.ver = 1;

  return m_iMsgLen;
}

int DccMsgParser::setMsgBuf(char *pBuf, int iLen)
{
    memcpy(m_sMsgBuf, pBuf, iLen);
    m_iMsgLen = ntohl(*((ACE_UINT32*)(m_sMsgBuf)))& 0x00ffffff;
    m_oMsg->hdr.ver = 1;
  
    return m_iMsgLen;
}


void  DccMsgParser::getMsgBuf(std::string &msg_buf)
{
  msg_buf.assign(m_sMsgBuf, m_iMsgLen);
}

void DccMsgParser::getMsgBuf(char *pBuf, int *piLen)
{
    memcpy(pBuf, m_sMsgBuf, m_iMsgLen);
    *piLen = m_iMsgLen;
}

//int DccMsgParser::getAvpValue( std::vector<int> &vAvpCode, std::vector<int> &vAvpPos)
int DccMsgParser::getAvpValue( int *vAvpCode, int *vAvpPos,int iCnt)
{
	int  i, j;
  int counter;
  m_pCurr = m_sMsgBuf+20;
  m_iCurrLen = m_iMsgLen - 20;
  for( i  = 0 ; i  <   iCnt ; i ++ )
  {
  	if( i == iCnt - 1)
  		(*(vAvpPos+i)) = 100;
    if( getAvpValuePos((*(vAvpCode+i)), (*(vAvpPos+i)), counter))
    {
      if( i == iCnt - 1 )
      {
        return counter;
      }
    }
    else
    {
    	if( i <  iCnt - 1 )
    		break;
      else
      	return counter;
    }
//    printf("[%d]:counter=%d\n", i, counter);
  }
  return 0 ;

}

int  DccMsgParser::getAvpValue( int avp_code)
{
	int counter;
  m_pCurr = m_sMsgBuf+20;
  m_iCurrLen = m_iMsgLen - 20;
  getAvpValuePos(avp_code, 100, counter);
  return counter ;
}


//int DccMsgParser::getAvpValue( std::vector<int> &vAvpCode, std::vector<int> &vAvpPos, ACE_INT32 &data)
int DccMsgParser::getAvpValue( int *vAvpCode, int *vAvpPos,int iCnt, ACE_INT32 &data)
{
	int  i, j;
	int  counter;
  m_pCurr = m_sMsgBuf+20;
  m_iCurrLen = m_iMsgLen - 20;
  for( i  = 0 ; i  <   iCnt ; i ++ )
  {
    if( getAvpValuePos((*(vAvpCode+i)), (*(vAvpPos+i)), counter))
    {
      if( i == iCnt - 1 )
      {
        data = ntohl(*((ACE_INT32*)(m_pCurr)));
        return 1 ;
      }
    }
    else
    	break;
  }
  return 0 ;
}


//int  DccMsgParser::getAvpValue( std::vector<int> &vAvpCode, ACE_INT32 &data)
int DccMsgParser::getAvpValue( int *vAvpCode,int iCnt, ACE_INT32 &data)
{
  int  i, j;
  int  counter;
  m_pCurr = m_sMsgBuf+20;
  m_iCurrLen = m_iMsgLen - 20;
  for( i  = 0 ; i  <   iCnt ; i ++ )
  {
    if( getAvpValuePos((*(vAvpCode+i)), 0, counter))
    {
      if( i == iCnt - 1 )
      {
        data = ntohl(*((ACE_INT32*)(m_pCurr)));
        return 1 ;
      }
    }
    else
    	break;
  }

 	return 0;
}


//int DccMsgParser::getAvpValue( std::vector<int> &vAvpCode, std::vector<int> &vAvpPos, ACE_UINT32 &data)
int DccMsgParser::getAvpValue( int *vAvpCode, int *vAvpPos,int iCnt, ACE_UINT32 &data)
{
	int  i, j;
	int  counter;
  m_pCurr = m_sMsgBuf+20;
  m_iCurrLen = m_iMsgLen - 20;
  for( i  = 0 ; i  <   iCnt ; i ++ )
  {
    if( getAvpValuePos((*(vAvpCode+i)), (*(vAvpPos+i)), counter))
    {
      if( i == iCnt - 1 )
      {

        data = ntohl(*((ACE_UINT32*)(m_pCurr)));

        return 1 ;
      }
    }
    else
    	break;
  }
  return 0 ;
}

//int  DccMsgParser::getAvpValue( std::vector<int> &vAvpCode, ACE_UINT32 &data)
int  DccMsgParser::getAvpValue( int *vAvpCode, int iCnt, ACE_UINT32 &data)
{
  int  i, j;
  int  counter;
  m_pCurr = m_sMsgBuf+20;
  m_iCurrLen = m_iMsgLen - 20;
  for( i  = 0 ; i  <   iCnt; i ++ )
  {
    if( getAvpValuePos((*(vAvpCode+i)), 0, counter))
    {
      if( i == iCnt - 1 )
      {
        data = ntohl(*((ACE_UINT32*)(m_pCurr)));
        return 1 ;
      }
    }
    else
    	break;
  }

 	return 0;
}

//int DccMsgParser::getAvpValue( std::vector<int> &vAvpCode, std::vector<int> &vAvpPos, std::string &data)
int DccMsgParser::getAvpValue( int *vAvpCode, int *vAvpPos,int iCnt, std::string &data)
{
	int  i, j;
	int counter;
  m_pCurr = m_sMsgBuf+20;
  m_iCurrLen = m_iMsgLen - 20;
  for( i  = 0 ; i  <   iCnt ; i ++ )
  {
    if( getAvpValuePos((*(vAvpCode+i)), (*(vAvpPos+i)), counter))
    {
      if( i == iCnt - 1 )
      {
        data.assign(m_pCurr, m_iCurrLen);

        return 1 ;
      }
    }
    else
    	break;
  }
  return 0 ;
}


//int  DccMsgParser::getAvpValue( std::vector<int> &vAvpCode, std::string &data)
int DccMsgParser::getAvpValue( int *vAvpCode,int iCnt, std::string &data)
{
	int  i, j;
	int counter;
	m_pCurr = m_sMsgBuf+20;
	m_iCurrLen = m_iMsgLen - 20;
	for( i  = 0 ; i  <   iCnt ; i ++ )
	{
	if( getAvpValuePos((*(vAvpCode+i)), 0, counter))
	{
	  if( i == iCnt - 1 )
	  {

		data.assign(m_pCurr, m_iCurrLen);
		return 1 ;
	  }
	}
	else
		break;
	}
	return 0 ;

}

//int DccMsgParser::getAvpValue( std::vector<int> &vAvpCode, std::vector<int> &vAvpPos, ACE_UINT64 &data)
int DccMsgParser::getAvpValue( int *vAvpCode, int *vAvpPos,int iCnt,  ACE_UINT64 &data)
{
	int  i, j;
  int counter;
  m_pCurr = m_sMsgBuf+20;
  m_iCurrLen = m_iMsgLen - 20;
  for( i  = 0 ; i  <   iCnt ; i ++ )
  {
    if( getAvpValuePos((*(vAvpCode+i)), (*(vAvpPos+i)), counter))
    {
      if( i == iCnt - 1 )
      {
        data = ntohq(*((ACE_UINT64*)(m_pCurr)));

        return 1;
      }
    }
    else
    	break;
  }
  return 0 ;

}

int DccMsgParser::getAvpValue( int avp_code, int pos, ACE_INT32 &data)
{
	int  j;
	int counter;
	m_pCurr = m_sMsgBuf+20;
	m_iCurrLen = m_iMsgLen - 20;
	if( getAvpValuePos(avp_code, pos, counter))
	{
		data = ntohl(*((ACE_INT32*)(m_pCurr)));

		return 1 ;
	}
	return 0 ;
}

int DccMsgParser::getAvpValue( int avp_code, int pos, ACE_UINT32 &data)
{
	int  j;
	int counter;
  m_pCurr = m_sMsgBuf+20;
  m_iCurrLen = m_iMsgLen - 20;
  if( getAvpValuePos(avp_code, pos, counter))
  {
    data = ntohl(*((ACE_UINT32*)(m_pCurr)));

    return 1 ;
  }
  return 0 ;
}

int  DccMsgParser::getAvpValue( int avp_code, ACE_INT32 &data)
{
	int  j;
	int counter;
	m_pCurr = m_sMsgBuf+20;
	m_iCurrLen = m_iMsgLen - 20;
	if( getAvpValuePos(avp_code, 0, counter))
	{
		data = ntohl(*((ACE_INT32*)(m_pCurr)));

		return 1 ;
	}
	return 0 ;

}

int  DccMsgParser::getAvpValue( int avp_code, ACE_UINT32 &data)
{
	int  j;
	int counter;
	m_pCurr = m_sMsgBuf+20;
	m_iCurrLen = m_iMsgLen - 20;
	if( getAvpValuePos(avp_code, 0, counter))
	{
		data = ntohl(*((ACE_UINT32*)(m_pCurr)));

		return 1 ;
	}
	return 0 ;

}

int DccMsgParser::getAvpValue( int avp_code, int pos, std::string &data)
{
	int  j;
	int counter;
	m_pCurr = m_sMsgBuf+20;
	m_iCurrLen = m_iMsgLen - 20;
	if( getAvpValuePos(avp_code, pos, counter))
	{
		data.assign(m_pCurr, m_iCurrLen);

		return 1 ;
	}
	return 0 ;
}

int  DccMsgParser::getAvpValue( int avp_code, std::string &data)
{
	int  j;
	int counter;
	m_pCurr = m_sMsgBuf+20;
	m_iCurrLen = m_iMsgLen - 20;
	if( getAvpValuePos(avp_code, 0, counter))
	{
		data.assign(m_pCurr, m_iCurrLen);

		return 1 ;
	}
	return 0 ;
}


int DccMsgParser::getAvpValuePos( int curr_avp_code, int pos, int &counter)
{
	int len, j;
	int avp_code;
	char *p = m_pCurr;
	for( counter = 0 ; p - m_pCurr < m_iCurrLen  ; )
	{
		len = ntohl(*((ACE_UINT32*)(p+4)))& 0x00ffffff;
		avp_code = ntohl(*((ACE_UINT32*)(p)));

		if( ntohl(*((ACE_UINT32*)(p))) == curr_avp_code )
		{
			if(  pos == counter  )
		  {
			  if( *(p+4) & 0x80 )
			  {
			  m_pCurr = p+12;
			  m_iCurrLen = len - 12;
			}
			else
			{
			  m_pCurr = p+8;
			  m_iCurrLen = len - 8;
			}
			  counter ++ ;
			return 1;
		  }
			counter ++ ;
		}

		p += len ;
		if( len%4)
		  p  += 4 - len % 4 ;
	}
	return 0;
}

//获取diameter消息头
AAADiameterHeader& DccMsgParser::getDiameterHeaderX()
{
	m_oMsg->hdr.ver = AAA_PROTOCOL_VERSION;
	m_oMsg->hdr.length = m_iMsgLen;

    m_oMsg->hdr.flags.r = m_sMsgBuf[4] & 0x80;
    m_oMsg->hdr.flags.p = m_sMsgBuf[4] & 0x40;
    m_oMsg->hdr.flags.e = m_sMsgBuf[4] & 0x20;
    m_oMsg->hdr.flags.t = m_sMsgBuf[4] & 0x10;
    m_oMsg->hdr.code = (*((ACE_UINT32*)(m_sMsgBuf+1*4))) & 0x00ffffff;
    m_oMsg->hdr.appId = (*((ACE_UINT32*)(m_sMsgBuf+2*4)));
    m_oMsg->hdr.hh = (*((ACE_UINT32*)(m_sMsgBuf+3*4)));
    m_oMsg->hdr.ee = (*((ACE_UINT32*)(m_sMsgBuf+4*4)));;

	return m_oMsg->hdr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////
