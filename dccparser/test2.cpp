#include <string>
#include <iostream>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include "diameter_parser_api.h"
#include "comlist.h"

using namespace std;

#define GET_DATA_REF(dataType, data, containerEntryPtr) \
        dataType &data = (containerEntryPtr)->dataRef(Type2Type<dataType>())


static int disp_flag = 0;
static int test_flag = 0;

static void print_header(AAADiameterHeader &h)
{
  cout << "version = " << (int)h.ver << endl;
  cout << "length=" << h.length << "\n";
  cout << "flags(r,p,e,t)=(" 
       << (int)h.flags.r << "," 
       << (int)h.flags.p << ","
       << (int)h.flags.e << ","
       << (int)h.flags.t << ")" << endl;
  cout << "applicationId = " << h.appId << endl;
  cout << "h-h id = " << h.hh << endl;
  cout << "e-e id = " << h.ee << endl;
}

void disp_buf(unsigned char *buf, int len)
{
    int i ;
    printf("buf start:");
    for( i = 0 ; i < len; i ++ ) 
    {
  	  if( i%16 == 0 ) 
  		  printf("\n");
  	  printf("%02X ", buf[i]&0xff);
    }		
    printf("\nbuf end\n");
}

int deleteAvpValue(AAAAvpContainerList* acl1, char *avp_name1)
{
   AAAAvpContainer* c;
   int quit_flag = 0;
   std::string::size_type pos, start_pos ;
   std::string avpName, avp_name = std::string(avp_name1);
   std::string::size_type pos1, pos2, pos3;
   AAAAvpContainerList* acl = acl1;
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
					printf("1.avpName=%s,size:[%d],pos:[%d],dataType:[%d],acl releaseContainers\n", avpName.c_str(), c->size(), pos, (*c)[pos]->dataType());
					acl->releaseContainers();
					pos = 0;
					c->resize(0);
				}
				else
				{
					printf("2.avpName=%s,size:[%d],pos:[%d],dataType:[%d],c releaseEntries\n", avpName.c_str(),c->size(), pos, (*c)[pos]->dataType());
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
int GetAvpValue(AAAAvpContainerList* acl1, char *avp_name1, T &data)
{
   AAAAvpContainer* c;
   int quit_flag = 0, pos, start_pos ; 
   std::string avpName, avp_name = std::string(avp_name1);
   int pos1, pos2, pos3;
   AAAAvpContainerList* acl = acl1;
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
    	  avpName = avp_name.substr(start_pos, 500);;

//      avpName = avp_name.substr(start_pos, 500);;
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
   			printf("pos[%d] is large size[%d]\n", pos, c->size());
			  return -1;
	  	}
   	}
   	else 
   	{
   		printf("avpName[%s] not found\n", avpName.c_str());
   		return -1;
    }
  }	
  return 0;	   	
   
}

template<class T>
int SetAvpValue(AAAAvpContainerList *acl1, char *avp_name1, T &data, AAA_AVPDataType t)
{
  AAAAvpContainerManager cm;
  AAAAvpContainerEntryManager em;
  AAAAvpContainerEntry *e;
  int quit_flag = 0, ret = 0, pos, start_pos, flag, set_flag ; 
  std::string avpName, avp_name = std::string(avp_name1);
  int pos1, pos2, pos3;
  AAAAvpContainerList *acl = acl1;
  AAAAvpContainerList *tp_list = NULL;
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
//    printf("avpName:[%s]\n", avpName.c_str());
    if( !flag && (c = acl->search(avpName.c_str()) ) )
    {
//    	printf("1.avpName:[%s]\n", avpName.c_str());
      if( c->size() > pos ) 
      {
    	  if( ((*c)[pos]->dataType()) == AAA_AVP_GROUPED_TYPE ) 
    	  {
    		  acl = ((*c)[pos])->dataPtr(Type2Type<AAAAvpContainerList>());
    		  pos = 0;
    		  set_flag = 1 ;
//    			printf("2.avpName:[%s],set_flag=1\n", avpName.c_str());
    	  }
    	  else 
    	  {
//    			printf("3.avpName:[%s],set_value\n", avpName.c_str());
    		  GET_DATA_REF(T, tmp, (*c)[pos]);
    		  pos = 0;
    		  tmp = data;
    		  return 1;
    	  }
      }
      else 
      {
//    		printf("avpName=%s,pos[%d] is large size, [%d]\n", avpName.c_str(), pos, c->size());
        if( !quit_flag ) 
    	  {
//    		printf("11.avpName:[%s],set_flag\n", avpName.c_str());
    		  e = em.acquire(AAA_AVP_GROUPED_TYPE);
    		  acl = e->dataPtr(Type2Type<diameter_grouped_t>());
    	    c->add(e);
    	  }
    	  else 
    	  {
    		  e = em.acquire(t);
    		  GET_DATA_REF(T, tp, e);
      	  c->add(e);
     	    tp = data;
    		  ret = 1;
    	  }
    	  flag = 1 ;
      }
    }
    else 
    {
//    	printf("2.avpName:[%s]\n", avpName.c_str());
      c = cm.acquire(avpName.c_str());
      if( !quit_flag ) 
      {
//    		printf("5.avpName:[%s],set_flag\n", avpName.c_str());
    	  e = em.acquire(AAA_AVP_GROUPED_TYPE);
    	  tp_list = e->dataPtr(Type2Type<diameter_grouped_t>());
    	  c->add(e);
     	}
      else 
      {
//    		printf("4.avpName:[%s],set_value\n", avpName.c_str());
//    		cout << "data:t" << t << ":" << data << endl;
    	  e = em.acquire(t);
    	  GET_DATA_REF(T, tp, e);
      	c->add(e);
     	  tp = data;
//    		cout << "data:" << data << ":" << tp << endl;
    	  ret = 1;
      }
      	      	
      if( acl ) 
      {
//        printf("2.acl add\n");
        acl->add(c);
    	}
      acl = tp_list;
    	flag = 1 ;
    }
    
  }	
  return ret;	   	
   
}

int GetGroupedAvpNums(AAAAvpContainerList *acl1, char *avp_name)
{
   AAAAvpContainer* c;
   char tp_name[300], *p;
   int i , flag ;
   AAAAvpContainerList *acl = acl1;
   for( i = 0, flag = 0; ; i ++  ) 
   {
   	if( (p = (char *) strstr(avp_name, ".")) )
   	{
        		memset(tp_name, 0, 200);
        		memcpy(tp_name, avp_name, p - avp_name);
        		strcpy(avp_name, tp_name);
       			if( (c = acl->search(tp_name)) )
			{
 				if( ((*c)[0]->dataType()) == AAA_AVP_GROUPED_TYPE ) 
				{
					acl = ((*c)[0])->dataPtr(Type2Type<AAAAvpContainerList>());
					strcpy(tp_name, p+1);
					strcpy(avp_name, tp_name);
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

void get_avp_name(std::vector<string>& sAvpName, std::string& avp_name)
{
	std::vector<string>::iterator itor;
	
	for (itor = sAvpName.begin(); itor != sAvpName.end(); itor ++ )
	{
		if( itor == sAvpName.begin()) 
			avp_name = (*itor);
		else 
			avp_name += "." + (*itor) ;
	
	}
}

void GetAvpData(AAAAvpContainerEntry *e, int i, int size, std::vector<string>& sAvpName) 
{
	std::string tp;
	std::string avp_name="";
	char sTmpBuf[20];
  char format[20];
  if( size > 1 ) 
    sprintf(format, "[%d]:", i ) ;
	else 
    strcpy(format, ":" ) ;
	switch( e->dataType()) 
	{
		  case AAA_AVP_INTEGER32_TYPE:
		  	{
//		  		std::cout << e->dataRef(Type2Type<diameter_integer32_t>()) << std::endl;
				     sprintf(sTmpBuf, "%d", e->dataRef(Type2Type<diameter_integer32_t>()));
				     get_avp_name(sAvpName, avp_name);
	  	       std::cout << avp_name << format <<  sTmpBuf << std::endl;
	   		   }
			     break;
		  case AAA_AVP_UINTEGER32_TYPE:
    			{
//		  		std::cout << e->dataRef(Type2Type<diameter_unsigned32_t>()) << std::endl;
				    sprintf(sTmpBuf, "%u", e->dataRef(Type2Type<diameter_unsigned32_t>()));
//		  		sTp += tp + "\n";
				    get_avp_name(sAvpName, avp_name);
		  		  std::cout << avp_name << format << sTmpBuf << std::endl;
    			}
			    break;
		
		  case AAA_AVP_INTEGER64_TYPE:
    			{
//		  		std::cout << e->dataRef(Type2Type<diameter_integer64_t>()) << std::endl;
//		  		sTp += tp + "\n";
				    sprintf(sTmpBuf, "%ld", e->dataRef(Type2Type<diameter_integer64_t>()));
				    get_avp_name(sAvpName, avp_name);
		  		  std::cout << avp_name << format << sTmpBuf << std::endl;
    			}
			    break;
		  case AAA_AVP_UINTEGER64_TYPE:
    			{
//		  		std::cout << e->dataRef(Type2Type<diameter_unsigned64_t>()) << std::endl;
				    sprintf(sTmpBuf, "%lu", e->dataRef(Type2Type<diameter_unsigned64_t>()));
				    get_avp_name(sAvpName, avp_name);
		  		  std::cout << avp_name<< format  << sTmpBuf << std::endl;
    			}
			    break;
		  case AAA_AVP_ENUM_TYPE:
    			{
//		  		std::cout << e->dataRef(Type2Type<diameter_integer32_t>()) << std::endl;
		  	    sprintf(sTmpBuf, "%d", e->dataRef(Type2Type<diameter_integer32_t>()));
				    get_avp_name(sAvpName, avp_name);
		  		  std::cout << avp_name << format  << sTmpBuf << std::endl;
    			}	
    			break;
		  case AAA_AVP_STRING_TYPE:
    			{
//		  		std::cout << e->dataRef(Type2Type<diameter_octetstring_t>()) << std::endl;
//		  		sTp += e->dataRef(Type2Type<diameter_octetstring_t>()) + "\n";
				    get_avp_name(sAvpName, avp_name);
		  		  std::cout << avp_name << format << e->dataRef(Type2Type<diameter_octetstring_t>()) << std::endl;
    			}
			    break;

		  case AAA_AVP_ADDRESS_TYPE:
    			{
//		  		std::cout << e->dataRef(Type2Type<diameter_ir_ipaddress_t>()) << std::endl;
//		  		sTp += e->dataRef(Type2Type<diameter_ipaddress_t>()) + "\n";
				    get_avp_name(sAvpName, avp_name);
		  		  std::cout << avp_name << format << e->dataRef(Type2Type<diameter_ipaddress_t>()) << std::endl;
    			}
			    break;
		  case AAA_AVP_UTF8_STRING_TYPE:
    			{
//		  		std::cout << e->dataRef(Type2Type<diameter_utf8string_t>()) << std::endl;
//		  		sTp += e->dataRef(Type2Type<diameter_utf8string_t>()) + "\n";
				    get_avp_name(sAvpName, avp_name);
		  		  std::cout << avp_name << format  << e->dataRef(Type2Type<diameter_utf8string_t>()) << std::endl;
    			}
			    break;
		  case AAA_AVP_DIAMID_TYPE:
    		  {
//		  		std::cout << e->dataRef(Type2Type<diameter_identity_t>()) << std::endl;
//		  		sTp += e->dataRef(Type2Type<diameter_identity_t>()) + "\n";
				    get_avp_name(sAvpName, avp_name);
		  		  std::cout << avp_name << format  << e->dataRef(Type2Type<diameter_identity_t>()) << std::endl;
    	    }
			    break;
		  case AAA_AVP_DIAMURI_TYPE:
		  case AAA_AVP_TIME_TYPE:
  	  case AAA_AVP_GROUPED_TYPE:
  	  case AAA_AVP_IPFILTER_RULE_TYPE:
		  case AAA_AVP_CUSTOM_TYPE:
	  	case AAA_AVP_DATA_TYPE:
//			std::cout << "data type[" << e->dataType() << "] noknown:" << std::endl;
			  break;
	}
	
}


void GetAllAvp(AAAAvpContainerList *orig_acl, std::vector<string>& sAvpName)
{
  AAAAvpContainerList *acl = orig_acl, *acl1;
  AAAAvpContainerList::iterator itor;
  AAAAvpContainer *c;	
  char tmp_buf[20];
  std::string tmp;
  int i;
  for (itor = acl->begin(); itor != acl->end(); itor ++ ) 
  {
  	
  	c = *itor;
	  for( i = 0 ; i < c->size() ; i ++ ) 
	  {
			
		  if( ((*c)[i]->dataType()) == AAA_AVP_GROUPED_TYPE ) 
  		{
			  if( c->size() > 1 ) 
			  {
//				cout << "\n\t[" << i << "]\n";
          sprintf(tmp_buf, "[%d]", i );
				  tmp = c->getAvpName() +std::string(tmp_buf);
				  sAvpName.push_back(tmp);
			  }
			  else 
			  {
				  sAvpName.push_back(c->getAvpName());
//				cout << ".";
			  }
			  acl1 = ((*c)[i])->dataPtr(Type2Type<AAAAvpContainerList>());
  			GetAllAvp(acl1, sAvpName);
  			sAvpName.pop_back();
  		}
  		else 
  		{
//  			sTp += ":";
//  			cout << ":" ;
			  sAvpName.push_back(c->getAvpName());
		  	GetAvpData((*c)[i], i, c->size(), sAvpName);
			  sAvpName.pop_back();
		  	
  		}
  	}
  }
}

static void 
set_buf(unsigned char *buf, int size)
{
	  unsigned char *p;
//	buf[0] = 100;          // version 5011
//	memset(&buf[1], 0, 3); // invalid msg length 5015
//  buf[4] |= 0x04;        // command flags 5013
//    buf[4] |= 0x20;        // r e bit invalid 3008
  
//  memcpy(&buf[5], "\x00\x11\x0f", 3);  // unsupport command code 3001
//    buf[24] |= 0x02;     //3009  Invalid AVP Bits
//  p = &buf[20];
//  *((ACE_UINT32*)(p)) = htonl(9999999);  5001 标志Mbit的avp不支持

//   buf[24] = 0 ;   //5016 Invalid AVP Bit combo

//    p = &buf[20];
//    *((ACE_UINT32*)(p)) = htonl(9999999);  // avp not allowed  5008

//     memcpy(&buf[25], "\x00\x00\x00", 3);  //invalid avp length  5014
//     memcpy(&buf[25], "\x00\x00\x00", 3);  //invalid avp length  5014

}

static int
stest(unsigned char *buf, int size)
{
 
//  hdr_flag flag = {1,0,0,1};
  hdr_flag flag = {1,0,0,1};
  AAADiameterHeader h(1, 0, flag, 9999999, 0, 1, 10);
  
  AAAMessage msg;
  AAAMessageBlock *aBuffer;
  msg.hdr = h;
  HeaderParser hp;
  aBuffer = AAAMessageBlock::Acquire((char*)buf, size);
  hp.setRawData(aBuffer);
  hp.setAppData(&msg.hdr);
  hp.setDictData(PARSE_STRICT);

  /* Check validity of flags and gets a AAACoand structure */
  try {
    hp.parseAppToRaw();
  }
  catch (AAAErrorStatus st)
  {
      std::cout << "header error" << std::endl;;
      aBuffer->Release();
      return -1;
  }
  ACE_INT32 int32;
  int32 = 3005;
  setResultCode(h.flags.r, int32);

  SetAvpValue(&msg.acl, "Result-Code", int32, AAA_AVP_UINTEGER32_TYPE);

  std::string tmp_str;
  tmp_str = "host1";
  SetAvpValue(&msg.acl, "Destination-Host", tmp_str, AAA_AVP_DIAMID_TYPE);


  int32 = 1;
  SetAvpValue(&msg.acl, "Acct-Application-Id", int32, AAA_AVP_UINTEGER32_TYPE);

  tmp_str = "proxy-host1";
  SetAvpValue(&msg.acl, "Proxy-Info[0].Proxy-Host", tmp_str, AAA_AVP_DIAMID_TYPE);

  tmp_str = "proxy-state1";
  SetAvpValue(&msg.acl, "Proxy-Info[0].Proxy-State", tmp_str, AAA_AVP_STRING_TYPE);

  tmp_str = "proxy-host2";
  SetAvpValue(&msg.acl, "Proxy-Info[1].Proxy-Host", tmp_str, AAA_AVP_DIAMID_TYPE);

  tmp_str = "proxy-state2";
  SetAvpValue(&msg.acl, "Proxy-Info[1].Proxy-State", tmp_str, AAA_AVP_STRING_TYPE);

  tmp_str = "rre1";
  SetAvpValue(&msg.acl, "Route-Record", tmp_str, AAA_AVP_STRING_TYPE);

  int32 = 1;
  SetAvpValue(&msg.acl, "CC-Request-Type", int32, AAA_AVP_ENUM_TYPE);

  int32 = 1;
  SetAvpValue(&msg.acl, "CC-Request-Number", int32, AAA_AVP_UINTEGER32_TYPE);

  tmp_str = "rrversion1.in@chinateleco.com";
  SetAvpValue(&msg.acl, "Service-Context-Id", tmp_str, AAA_AVP_STRING_TYPE);

  tmp_str = "SCP001.chinateleco.com";
  SetAvpValue(&msg.acl, "Origin-Host", tmp_str, AAA_AVP_DIAMID_TYPE);

  tmp_str = "chinateleco.com";
  SetAvpValue(&msg.acl, "Origin-Realm", tmp_str, AAA_AVP_DIAMID_TYPE);


  tmp_str = "chinateleco.com";
  SetAvpValue(&msg.acl, "Destination-Realm", tmp_str, AAA_AVP_DIAMID_TYPE);

  int32 = 4;
  SetAvpValue(&msg.acl, "Auth-Application-Id", int32, AAA_AVP_UINTEGER32_TYPE);

/*  tmp_str = "SCP001.chinateleco.co;1876543210;111";
  SetAvpValue(&msg.acl, "Session-Id", tmp_str, AAA_AVP_UTF8_STRING_TYPE);

  tmp_str = "SCP001.chinateleco.co;1876543210;222";
  SetAvpValue(&msg.acl, "Session-Id[1]", tmp_str, AAA_AVP_UTF8_STRING_TYPE);
*/
  tmp_str = "SCP001.chinateleco.co;1876543210;333";
  SetAvpValue(&msg.acl, "Session-Id[2]", tmp_str, AAA_AVP_UTF8_STRING_TYPE);

  unsigned long int data = 356;
  SetAvpValue(&msg.acl, "Multiple-Services-Credit-Control.Used-Service-Unit.CC-Output-Octets", data, AAA_AVP_UINTEGER64_TYPE);
  data = 678;
  SetAvpValue(&msg.acl, "Multiple-Services-Credit-Control.Used-Service-Unit[1].CC-Output-Octets", data, AAA_AVP_UINTEGER64_TYPE);

  data = 345;
  SetAvpValue(&msg.acl, "Multiple-Services-Credit-Control.Used-Service-Unit[1].CC-Input-Octets", data, AAA_AVP_UINTEGER64_TYPE);

  data = 999;
  SetAvpValue(&msg.acl, "Multiple-Services-Credit-Control.Used-Service-Unit[1].CC-Input-Octets[1]", data, AAA_AVP_UINTEGER64_TYPE);

  std::string str("SCP001.chinateleco.co");
  SetAvpValue(&msg.acl, "Example-String", str, AAA_AVP_STRING_TYPE);

  if( disp_flag ) 
  {
    printf("before delete ...\n");
    std::vector<string> avpName;
    avpName.clear();
    GetAllAvp(&msg.acl, avpName);

   
//    deleteAvpValue(&msg.acl, "Multiple-Services-Credit-Control.Used-Service-Unit[1]");
//    deleteAvpValue(&msg.acl, "Session-Id");
    printf("############After delete ...\n");
    avpName.clear();
    GetAllAvp(&msg.acl, avpName);

  }
//  ::setDictHandle(msg, "", "", "");
  PayloadParser pp;
  pp.setRawData(aBuffer);
  pp.setAppData(&msg.acl);
  pp.setDictData(msg.hdr.getDictHandle());

  try 
   { 
     pp.parseAppToRaw();
   }
  catch (AAAErrorStatus st)
    {
      std::cout << "\nassemble failed" << std::endl;
      msg.acl.releaseContainers();
      aBuffer->Release();
      return -1;
    }

  msg.hdr.length = aBuffer->wr_ptr() - aBuffer->base();

  /* The second calll of hp.set() sets the actual essage length */
  try {
    hp.parseAppToRaw();
  }
  catch (AAAErrorStatus st)
    {
      std::cout << "header error" << std::endl;
      msg.acl.releaseContainers();
      aBuffer->Release();
      return -1;
    }

  if( disp_flag  ) 
  {
    std::cout << "assemble " << msg.hdr.getCommandName() 
	    << " success. total length = " << msg.hdr.length << std::endl;
    disp_buf(buf,  msg.hdr.length);
  }
  msg.acl.releaseContainers();
  aBuffer->Release();
  return 0;

}

static void stest_command_fail(int code, unsigned char *buf, int size)
{
  unsigned char *p;
  buf[1] = 0 ;
  buf[2] = 0 ;
  buf[3] = 0x20;
  buf[4] &= 0x7F;
  memcpy(&buf[20], "\x00\x00\x01\x0C\x40\x00\x00\x0C", 8 ) ;
  p = &buf[28];
  *((ACE_UINT32*)(p)) = ntohl(code);
  disp_buf(buf, 32);
}

static void
stest_fail(AAAMessage &msg, int code, unsigned char *buf, int size)
{
 
  AAAMessageBlock *aBuffer;
  msg.hdr.flags.r = 0;

  HeaderParser hp;
  aBuffer = AAAMessageBlock::Acquire((char*)buf, size);
  hp.setRawData(aBuffer);
  hp.setAppData(&msg.hdr);
  hp.setDictData(PARSE_STRICT);

  /* Check validity of flags and gets a AAACoand structure */
  try {
    hp.parseAppToRaw();
  }
  catch (AAAErrorStatus st)
  {
      std::cout << "header error" << std::endl;;
      aBuffer->Release();
      exit(1);
  }
  ACE_INT32 int32;
  int32 = code;
  setResultCode(msg.hdr.flags.r, int32);
  SetAvpValue(&msg.acl, "Result-Code", int32, AAA_AVP_UINTEGER32_TYPE);

//  ::setDictHandle(msg, "");
  PayloadParser pp;
  pp.setRawData(aBuffer);
  pp.setAppData(&msg.acl);
  pp.setDictData(msg.hdr.getDictHandle());

  try 
  { 
    pp.parseAppToRaw();
  }
  catch (AAAErrorStatus st)
  {
      std::cout << "\nassemble failed" << std::endl;
      
      msg.acl.releaseContainers();
      aBuffer->Release();
      exit(1);
  }

  msg.hdr.length = aBuffer->wr_ptr() - aBuffer->base();

  /* The second calll of hp.set() sets the actual essage length */
  try {
    hp.parseAppToRaw();
  }
  catch (AAAErrorStatus st)
  {
      std::cout << "header error" << std::endl;
      msg.acl.releaseContainers();
      aBuffer->Release();
      exit(1);
  }

  if( disp_flag  ) 
  {
    std::cout << "assemble " << msg.hdr.getCommandName() 
	    << " success. total length = " << msg.hdr.length << std::endl;
    if( disp_flag ) 
    {
      disp_buf(buf, msg.hdr.length);
      std::vector<string> avpName;
      avpName.clear();
      GetAllAvp(&msg.acl, avpName);
    }
  }

  msg.acl.releaseContainers();
  aBuffer->Release();

}


static void
rtest(unsigned char *buf, int size)
{
  AAAMessage msg;
  AAAMessageBlock *aBuffer;
//  AAAAvpContainer* c;
  struct timeval start_timeval , end_timeval;
    
  gettimeofday(&start_timeval, NULL);
  msg.acl.releaseContainers();
  HeaderParser hp;
//  cout << "start rtest ..." << endl;
  aBuffer = AAAMessageBlock::Acquire((char*)buf, size);
  hp.setRawData(aBuffer);
  hp.setAppData(&msg.hdr);
  hp.setDictData(PARSE_STRICT);
  
  try {
    hp.parseRawToApp();
  }
  catch (AAAErrorStatus st) 
  {
      cout << "header error" << endl;
      int type, code;
      std::string avp;
      st.get(type, code, avp);
      std::cout << "Error type=" << type << ", code=" 
		    << code << ", name=" << avp << std::endl;
      aBuffer->Release();
      if( code == AAA_COMMAND_UNSUPPORTED ) 
        stest_command_fail(code, buf, size);
      else 
        stest_fail(msg, code, buf, size);
      return ;
  }

  if( disp_flag ) 
  {
    disp_buf(buf, msg.hdr.length);
    print_header(msg.hdr);
  }
  
/*  unsigned char *p=&buf[20];
  ACE_UINT32 avp_code;
  avp_code  = ntohl32(*((ACE_UINT32*)(p)));
  printf("supported h.code=%d\n", msg.hdr.code);
  if( msg.hdr.code == 272 || msg.hdr.code == 258 || msg.hdr..code == 274 ) 
  {
    printf("#######session-id avp code =%d\n", avp_code);
  	if( avp_code !=  263 )
    {
      AAA_LOG(LM_ERROR, "AVP not allowed [%d]\n", avp_code );
      st.set(NORMAL, AAA_AVP_NOT_ALLOWED);
      throw st;
    }
  } */
  
  
  setRequestFlag(msg.hdr.flags.r); 
//  avp_code = ntohl32(*((ACE_UINT32*)p));
  
  gettimeofday(&end_timeval, NULL);
//  std::cout << "########: hpget---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
//   			 << " s]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 

  gettimeofday(&start_timeval, NULL);

  PayloadParser pp;
  aBuffer->size(msg.hdr.length);
  pp.setRawData(aBuffer);
  pp.setAppData(&msg.acl);
  pp.setDictData(msg.hdr.getDictHandle());
  pp.setListNum(1);
  try {
    pp.parseRawToApp();
  }
  catch (AAAErrorStatus st) {
    std::cout << "payload parser error" << endl;
    int type, code;
    std::string avp;
    st.get(type, code, avp);
    std::cout << "Error type=" << type << ", code=" 
		<< code << ", name=" << avp << std::endl;
//    msg.acl.releaseContainers();
    aBuffer->Release();
    stest_fail(msg, code, buf, size);
    return ;
  }
  
   gettimeofday(&end_timeval, NULL);
//  ::setDictHandle(msg, "");
  aBuffer->rd_ptr(aBuffer->base() + HEADER_SIZE);
  aBuffer->size(msg.hdr.length);
  pp.setRawData(aBuffer);
  pp.setAppData(&msg.acl);

  pp.setDictData(msg.hdr.getDictHandle());
  pp.setListNum(3);
  try {
    pp.parseRawToApp();
  }
  catch (AAAErrorStatus st) {
    std::cout << "payload parser error" << endl;
    int type, code;
    std::string avp;
    st.get(type, code, avp);
    std::cout << "Error type=" << type << ", code=" 
		   << code << ", name=" << avp << std::endl;
  //  msg.acl.releaseContainers();
    aBuffer->Release();
    stest_fail(msg, code, buf, size);
    return ;
  } 
   gettimeofday(&end_timeval, NULL);
//   std::cout << "########: ppget---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
//   			 << " s]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 


  if( disp_flag  ) 
  {
    gettimeofday(&start_timeval, NULL);
    std::vector<string> avpName;
    avpName.clear();
    GetAllAvp(&msg.acl, avpName);
    gettimeofday(&end_timeval, NULL);
    std::cout << "########: GetAllAvp---period:[" << (end_timeval.tv_sec*1000000 + end_timeval.tv_usec - start_timeval.tv_sec*1000000 - start_timeval.tv_usec ) /1000 
   			 << " s]-[" << (end_timeval.tv_sec*1000000 + end_timeval.tv_usec - start_timeval.tv_sec*1000000 - start_timeval.tv_usec )  << " us]" << std::endl; 
    std::string sessionId;
//    int i;
    if( GetAvpValue(&msg.acl, "Session-Id[0]", sessionId) > 0 ) 
    {
      printf("[0]:%s\n", sessionId.c_str());
    }
    if( GetAvpValue(&msg.acl, "Session-Id[1]", sessionId) > 0 ) 
    {
      printf("[1]:%s\n", sessionId.c_str());
    	
    }  
    if( GetAvpValue(&msg.acl, "Session-Id[2]", sessionId) > 0 ) 
    {
      printf("[2]:%s\n", sessionId.c_str());
    }  

    ACE_UINT64 int64;
    gettimeofday(&start_timeval, NULL);
    GetAvpValue(&msg.acl, "Multiple-Services-Credit-Control.Used-Service-Unit.CC-Output-Octets", int64);
    gettimeofday(&end_timeval, NULL);
    std::cout << "########: 1.GetAvpValue---period:[" << (end_timeval.tv_sec*1000000 + end_timeval.tv_usec - start_timeval.tv_sec*1000000 - start_timeval.tv_usec ) /1000 
   			 << " s]-[" << (end_timeval.tv_sec*1000000 + end_timeval.tv_usec - start_timeval.tv_sec*1000000 - start_timeval.tv_usec )  << " us]" << std::endl; 
    printf("data=%d\n", int64);

    gettimeofday(&start_timeval, NULL);
    GetAvpValue(&msg.acl, "Multiple-Services-Credit-Control.Used-Service-Unit[1].CC-Output-Octets", int64);
    gettimeofday(&end_timeval, NULL);
    std::cout << "########: 2.GetAvpValue---period:[" << (end_timeval.tv_sec*1000000 + end_timeval.tv_usec - start_timeval.tv_sec*1000000 - start_timeval.tv_usec ) /1000 
   			 << " s]-[" << (end_timeval.tv_sec*1000000 + end_timeval.tv_usec - start_timeval.tv_sec*1000000 - start_timeval.tv_usec )  << " us]" << std::endl; 
    printf("data=%d\n", int64);

    gettimeofday(&start_timeval, NULL);
    GetAvpValue(&msg.acl, "Multiple-Services-Credit-Control.Used-Service-Unit[1].CC-Input-Octets", int64);
    gettimeofday(&end_timeval, NULL);
    std::cout << "########: 3.GetAvpValue---period:[" << (end_timeval.tv_sec*1000000 + end_timeval.tv_usec - start_timeval.tv_sec*1000000 - start_timeval.tv_usec ) /1000 
   			 << " s]-[" << (end_timeval.tv_sec*1000000 + end_timeval.tv_usec - start_timeval.tv_sec*1000000 - start_timeval.tv_usec )  << " us]" << std::endl; 
    printf("data=%d\n", int64);

    
  } 
  msg.acl.releaseContainers();
  aBuffer->Release();
//   printf("rtest exit...\n");

}

static void
rtest1(unsigned char *buf, int size)
{
  AAAMessage msg;
  AAAMessageBlock *aBuffer;
//  AAAAvpContainer* c;
  struct timeval start_timeval , end_timeval;
    
  gettimeofday(&start_timeval, NULL);
  msg.acl.releaseContainers();
  HeaderParser hp;
//  cout << "start rtest ..." << endl;
  aBuffer = AAAMessageBlock::Acquire((char*)buf, size);
  hp.setRawData(aBuffer);
  hp.setAppData(&msg.hdr);
  hp.setDictData(PARSE_STRICT);
  
  try {
    hp.parseRawToApp();
  }
  catch (AAAErrorStatus st) 
  {
      cout << "header error" << endl;
      int type, code;
      std::string avp;
      st.get(type, code, avp);
      std::cout << "Error type=" << type << ", code=" 
		    << code << ", name=" << avp << std::endl;
      aBuffer->Release();
      return ;
  }

  if( disp_flag ) 
  {
    disp_buf(buf, msg.hdr.length);
    print_header(msg.hdr);

  }
  unsigned char *p=&buf[20];
  ACE_UINT32 avp_code;
  
  setRequestFlag(msg.hdr.flags.r); 
  avp_code = ntohl(*((ACE_UINT32*)p));
  
  gettimeofday(&end_timeval, NULL);
//  std::cout << "########: hpget---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
//   			 << " s]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 

  gettimeofday(&start_timeval, NULL);

  PayloadParser pp;
  aBuffer->size(msg.hdr.length);
  pp.setRawData(aBuffer);
  pp.setAppData(&msg.acl);
  pp.setDictData(msg.hdr.getDictHandle());
  pp.setListNum(1);
  try {
    pp.parseRawToApp();
  }
  catch (AAAErrorStatus st) {
    std::cout << "payload parser error" << endl;
    int type, code;
    std::string avp;
    st.get(type, code, avp);
    std::cout << "Error type=" << type << ", code=" 
		<< code << ", name=" << avp << std::endl;
//    msg.acl.releaseContainers();
    aBuffer->Release();
    return ;
  }
  
   gettimeofday(&end_timeval, NULL);
//  ::setDictHandle(msg, "");
  aBuffer->rd_ptr(aBuffer->base() + HEADER_SIZE);
  aBuffer->size(msg.hdr.length);
  pp.setRawData(aBuffer);
  pp.setAppData(&msg.acl);

  pp.setDictData(msg.hdr.getDictHandle());
  pp.setListNum(3);
  try {
    pp.parseRawToApp();
  }
  catch (AAAErrorStatus st) {
    std::cout << "payload parser error" << endl;
    int type, code;
    std::string avp;
    st.get(type, code, avp);
    std::cout << "Error type=" << type << ", code=" 
		   << code << ", name=" << avp << std::endl;
  //  msg.acl.releaseContainers();
    aBuffer->Release();
    return ;
  } 
   gettimeofday(&end_timeval, NULL);
//   std::cout << "########: ppget---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
//   			 << " s]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 

  if( disp_flag ) 
  {
    gettimeofday(&start_timeval, NULL);
    std::vector<string> avpName;
    avpName.clear();
    GetAllAvp(&msg.acl, avpName);
    gettimeofday(&end_timeval, NULL);
    std::cout << "########: GetAllAvp---period:[" << (end_timeval.tv_sec*1000000 + end_timeval.tv_usec - start_timeval.tv_sec*1000000 - start_timeval.tv_usec ) /1000 
   			 << " s]-[" << (end_timeval.tv_sec*1000000 + end_timeval.tv_usec - start_timeval.tv_sec*1000000 - start_timeval.tv_usec )  << " us]" << std::endl; 
  } 
  msg.acl.releaseContainers();
  aBuffer->Release();

}


int
main(int argc, char** argv)
{
  unsigned char sbuf[1024];
  AAADictionaryManager d;
  char config_file[100];
  int counter, num;
  char test_dict_file[80];
  char dict_file[80];
  // Start logging with specifying the use of syslog
  //  ACE_Log_Msg::instance()->open(argv[0], ACE_Log_Msg::SYSLOG);

  // Read dictionary file.
  
  sprintf(config_file, "%s/config/dictionary.xml", getenv("HOME"));
  d.init("dictionary.xml");
  printf("comlist size=%d\n", AAACommandList::instance()->size());
  strcpy(test_dict_file, "dictionary-test.xml");
  strcpy(dict_file, "dictionary.xml");
  if(argc > 1 ) 
  	num = atoi(argv[1]);
  else 
  	num = 1;
  	
  if( argc > 2 ) 
    disp_flag = atoi(argv[2]);
  if( argc > 3 ) 
    test_flag = atoi(argv[3]);
    
  struct timeval start_timeval , end_timeval;
  printf("start parser_test2 ...\n");
  gettimeofday(&start_timeval, NULL);
  for( counter = 0 ; counter < num ; counter ++ )
  {
    if( stest(sbuf, sizeof(sbuf)) < 0 ) 
    {
      if( (counter != 0 ) && ( counter % 1000)  == 0 )
      { 
        gettimeofday(&end_timeval, NULL);
        std::cout << "########PID:[" << getpid() << "],counter:" << counter << ",diff_time:[" << ((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
		    << " ms]-[" << ((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us],speed:[" <<  (((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 )/1000 << "ms],[" << (((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) )/1000 << " us]" << std::endl; 
        gettimeofday(&start_timeval, NULL);
      }	
      continue;
    }
    if( test_flag ) 
      set_buf(sbuf, sizeof(sbuf));

    if( test_flag ) 
    {
      ::RemoveDictionary();
      d.init(dict_file);
    }
    rtest(sbuf, sizeof(sbuf));
    if( test_flag ) 
    {
      printf("start rtest1 ...\n");
      rtest1(sbuf, sizeof(sbuf));
    } 
    if( (counter != 0 ) && ( counter % 1000)  == 0 )
    { 
      gettimeofday(&end_timeval, NULL);
      std::cout << "########PID:[" << getpid() << "],counter:" << counter << ",diff_time:[" << ((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
		  << " ms]-[" << ((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us],speed:[" <<  (((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 )/1000 << "ms],[" << (((end_timeval.tv_sec-start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) )/1000 << " us]" << std::endl; 
      gettimeofday(&start_timeval, NULL);
    }	
  }
  printf("program exit...\n");
  return (0);
}
