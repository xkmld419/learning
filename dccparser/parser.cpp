#include <list>
#include <typeinfo>
#include "comlist.h"
#include "avplist.h"
#include "q_avplist.h"
#include "parser.h"
#include "parser_avp.h"
#include "parser_q_avplist.h"
#include "resultcodes.h"    /* file location will be changed */
#include "diameter_parser_api.h"

static void transform(std::string &str)
{

        int i;
        char tmp[50];
        strcpy(tmp, str.c_str());
        for( i = 0 ; tmp[i] != 0 ; i ++)
           tmp[i] = toupper(tmp[i]);
        str = tmp;
}


/* ACE_UINT32 buftohl32(char *buf2)
{
	ACE_UINT32 int32;
  char buf1[4];
  int i ; 
  for( i = 0 ; i < 4 ; i ++ ) 
  {
  	buf1[i] = buf2[4-i];
  }	
  memcpy(&int32, buf1, 4);
  return int32;
	
}

void hl32tobuf(ACE_UINT32 int32, char *buf1)
{
  char buf2[2];
  int i ; 
  memcpy(buf2, &int32, 4 ) ;
  for( i = 0 ; i < 4 ; i ++ ) 
  {
  	buf1[i] = buf2[4-i];
  }	
  memcpy(&int32, buf1, 4);

}

ACE_UINT32 ntohl32(ACE_UINT32 q)
{
	ACE_UINT32 int32;
  char buf1[4], buf2[4];
  memcpy(buf2, &q, 4);
  int i ; 
  for( i = 0 ; i < 4 ; i ++ ) 
  {
  	buf1[i] = buf2[4-i];
  }	
  memcpy(&int32, buf1, 4);
  return int32;
}*

 

ACE_UINT32 getlen(char *buf)
{
	ACE_UINT32 int32;
  int32 = buf[1]*65536 + buf[2]*256 + buf[3];
  return int32;

} 
*/

template<> void
HeaderParser::parseRawToApp()// throw(AAAErrorStatus)
{
  AAAMessageBlock *aBuffer = getRawData();
  AAADiameterHeader *aHeader = getAppData();
  ParseOption opt = getDictData();

  AAADiameterHeader &h = *aHeader;
  aBuffer->rd_ptr(aBuffer->base());
  char *p = aBuffer->rd_ptr();
  AAAErrorStatus st;
  AAACommand *com;
//  ACE_UINT32 avp_code;
  h.ver = *((AAA_UINT8*)(p));
  h.length = ntohl(*((ACE_UINT32*)(p))) & 0x00ffffff;
//  h.length = getlen(p);
  p += 4;
  h.flags.r = *((AAA_UINT8*)(p)) >> 7;
  h.flags.p = *((AAA_UINT8*)(p)) >> 6;
  h.flags.e = *((AAA_UINT8*)(p)) >> 5;
  h.flags.t = *((AAA_UINT8*)(p)) >> 4;
  h.flags.rsvd = (*((AAA_UINT8*)(p)))&0x0F;

//  h.code = getlen(p);
  h.code = ntohl(*((ACE_UINT32*)(p))) & 0x00ffffff;
  p += 4;
  h.appId = ntohl(*((ACE_UINT32*)(p)));
//  h.appId = buftohl32(p);
  p += 4;
  h.hh = ntohl(*((ACE_UINT32*)(p)));
//  h.hh = buftohl32(p);
  p += 4;
//  h.ee = buftohl32(p);
  h.ee = ntohl(*((ACE_UINT32*)(p)));
  p += 4;
  
/*  avp_code  = ntohl32(*((ACE_UINT32*)(p)));
  if( h.code == 272 || h.code == 258 || h.code == 274 || h.code == 9999999) 
  {
  	if( avp_code !=  263 )
    {
      AAA_LOG(LM_ERROR, "AVP not allowed [%d]\n", avp_code );
      st.set(NORMAL, AAA_AVP_NOT_ALLOWED);
      throw st;
    }
  }
*/  

  aBuffer->rd_ptr(p);

  if (opt == PARSE_LOOSE)
  {
    return;
  }

  if( h.ver != 1 ) 
  {
    AAA_LOG(LM_ERROR, "command (%d, r-flag=%1d, version=%d) version unsupported\n", 
	       h.code, h.flags.r, h.ver);
    st.set(NORMAL, AAA_UNSUPPORTED_VERSION);
    throw st;
  }
  if( h.length == 0 || h.length%4 ) 
  {
    AAA_LOG(LM_ERROR, "command (%d, r-flag=%1d, length=%d) invalid msg length \n", 
	       h.code, h.flags.r, h.length);
    st.set(NORMAL, AAA_INVALID_MESSAGE_LENGTH);
    throw st;
  }

  if( h.flags.rsvd ) 
  {
    AAA_LOG(LM_ERROR, "command (%d, r-flag=%1d, r-rsvd=%4d, length=%d) invalid bit \n", 
	       h.code, h.flags.r, h.flags.rsvd, h.length);
    st.set(NORMAL, AAA_INVALID_BIT_IN_HEADER);
    throw st;
  }
  
  if ((com = AAACommandList::instance()
       ->search(h.code, h.appId, h.flags.r)) == NULL)
  {
    AAA_LOG(LM_ERROR, "command (%d,r-flag=%1d) not found\n", 
	       h.code, h.flags.r);
    st.set(NORMAL, AAA_COMMAND_UNSUPPORTED);
    throw st;
  }

  if (com->flags.r && h.flags.e)
  {
    AAA_LOG(LM_ERROR, 
	      "command (%d) e flag cannot be set for request\n", 
	      h.code);
    st.set(NORMAL, AAA_INVALID_HDR_BITS);
    throw st;
  }

/*  if (com->flags.p == 0 && h.flags.p == 1)
  {
    AAA_LOG(LM_ERROR, 
	      "command (%d) contains mismatch in p flag\n", 
	      h.code);
    st.set(NORMAL, AAA_INVALID_HDR_BITS);
    throw st;
  }
*/
  /* special handling for answer message with error flag set */
/*   if (! com->flags.r && h.flags.e)
  {
    if (h.flags.p)
    {
      com = AAACommandList::instance()
        ->search("PROXYABLE-ERROR-Answer");
    }
    else
    {
      com = AAACommandList::instance()
        ->search("NON-PROXYABLE-ERROR-Answer");
    }
    if (com == NULL)
    {
      AAA_LOG(LM_ERROR, 
    		  "1.No way to handle answer with error\n");
      st.set(NORMAL, AAA_COMMAND_UNSUPPORTED);
      throw st;
    }
  }
*/  
  
  h.dictHandle = com;
}

template<> void
HeaderParser::parseAppToRaw()// throw(AAAErrorStatus)
{
  AAAMessageBlock *aBuffer = getRawData();
  AAADiameterHeader *aHeader = getAppData();
  ParseOption opt = getDictData();
  AAADiameterHeader &h = *aHeader;
  char *p = aBuffer->base();
  AAAErrorStatus st;
  AAACommand *com=0;

  aBuffer->wr_ptr(aBuffer->base());

  if (opt != PARSE_LOOSE)
  {

    if ((com = AAACommandList::instance()->
	    search(h.code, h.appId, h.flags.r)) == NULL)
    {
      AAA_LOG(LM_ERROR, "2.command (%d,r-flag=%1d,appid=%d) not found\n", 
    		  h.code, h.flags.r,h.appId);
      st.set(NORMAL, AAA_COMMAND_UNSUPPORTED);
      throw st;
    }

    if (com->flags.r && h.flags.e)
    {
      AAA_LOG(LM_ERROR, 
    		  "command (%d) e flag cannot be set for request\n", 
    		  h.code);
      st.set(NORMAL, AAA_INVALID_HDR_BITS);
      throw st;
    }

/*    if (com->flags.p == 0 && h.flags.p == 1)
    {
      AAA_LOG(LM_ERROR, 
    		  "command (%d) contains mismatch in p flag\n", 
    		  h.code);
      st.set(NORMAL, AAA_INVALID_HDR_BITS);
      throw st;
    }
*/
      /* special handling for answer message with error flag set */
/*    if (! com->flags.r && h.flags.e)
    {
      if (h.flags.p)
      {
        com = AAACommandList::instance()->search("PROXYABLE-ERROR-Answer");
      }
      else
      {
        com = AAACommandList::instance()->search("NON-PROXYABLE-ERROR-Answer");
      }
      if (com == NULL)
      {
        AAA_LOG(LM_ERROR, 
    	      "2.No way to handle answer with error\n");
        st.set(NORMAL, AAA_COMMAND_UNSUPPORTED);
        throw st;
      }
    }
*/    
  }

//  hl32tobuf(h.length, p);
  *((ACE_UINT32*)(p)) = ntohl(h.length);
  *((AAA_UINT8*)(p)) = h.ver;
  p += 4;

//  hl32tobuf(h.code, p);
  *((ACE_UINT32*)(p)) = ntohl(h.code);
  *((AAA_UINT8*)(p)) = 
    (h.flags.r << 7) | (h.flags.p << 6)  | 
    (h.flags.e << 5) | (h.flags.t << 4);;

  p += 4;
  
//  hl32tobuf(h.appId, p);
  *((ACE_UINT32*)(p)) = ntohl(h.appId);
  p += 4;
//  hl32tobuf(h.hh, p);
  *((ACE_UINT32*)(p)) = ntohl(h.hh);
  p += 4;
//  hl32tobuf(h.ee, p);
  *((ACE_UINT32*)(p)) = ntohl(h.ee);
  p += 4;

  aBuffer->wr_ptr(p);
  h.dictHandle = com;
}


void setDictHandle(AAAMessage &msg, const char *requestType, const char *serviceType, const char *serviceId)// throw(AAAErrorStatus)
{

	AAACommand *com;
  std::string scenario; 
  if( strcmp(serviceId, "") == 0  ) 
    scenario = std::string(serviceType) + "_" + std::string(requestType);
  else 
    scenario = std::string(serviceType) + "_" + std::string(requestType) + "_" + std::string(serviceId);

//  transform (scenario.begin(), scenario.end(), scenario.begin(), toupper);
  transform (scenario);
  if( (com = AAACommandList::instance()
    ->search(msg.hdr.code, msg.hdr.appId, msg.hdr.flags.r, -1, scenario.c_str())) == NULL)
  {
    AAA_LOG(LM_ERROR, "1.command (%d,r-flag=%1d,scenario=%s) not found\n", 
  	msg.hdr.code, msg.hdr.flags.r, scenario.c_str());
	  return ;
  }
#ifdef	DEBUG_TEST
   	AAA_LOG(LM_ERROR, "#######.command (%d,r-flag=%1d,scenario=%s) found\n", 
	       		msg.hdr.code, msg.hdr.flags.r, scenario.c_str());
#endif
	msg.hdr.setDictHandle(com);
}


template<> void
PayloadParser::parseRawToApp()// throw(AAAErrorStatus)
{
  AAAMessageBlock *aBuffer = getRawData();
  AAAAvpContainerList *acl = getAppData();
  AAADictionary *dict = (AAADictionary*)getDictData();

  QualifiedAvpListParser qc;
  qc.setRawData(aBuffer);
  qc.setAppData(acl);
  qc.setDictData(dict);
  qc.setListNum(getListNum());

  try {
    qc.parseRawToApp();
  }
  catch (AAAErrorStatus st) {
    AAA_LOG(LM_ERROR, "Parse error");
//    throw ;
    throw ;
  }
}

template<> void
PayloadParser::parseAppToRaw()// throw(AAAErrorStatus)
{
  AAAMessageBlock *aBuffer = getRawData();
  AAAAvpContainerList *acl = getAppData();
  AAADictionary *dict = (AAADictionary*)getDictData();

  memset(aBuffer->wr_ptr(), 0, 
		 aBuffer->size() - (size_t)aBuffer->wr_ptr() +
		 (size_t)aBuffer->base());

  QualifiedAvpListParser qc;
  qc.setRawData(aBuffer);
  qc.setAppData(acl);
  qc.setDictData(dict);
  qc.setListNum(getListNum());

  try {
    qc.parseAppToRaw();
  }
  catch (AAAErrorStatus st) {
    AAA_LOG(LM_ERROR, "Parse error");
    throw;
  }
}

#if 0
char* AAADiameterHeader::getCommandName()
{ return dictHandle ? ((AAACommand*)dictHandle)->name : NULL; }
#else
const char* AAADiameterHeader::getCommandName()
{ return dictHandle ? ((AAACommand*)dictHandle)->name.c_str() : NULL; }
#endif

template<> void
HeaderParserWithProtocol::parseRawToApp()// throw(AAAErrorStatus)
{
  AAAMessageBlock *aBuffer = getRawData();
  AAADiameterHeader *aHeader = getAppData();
  AAADictionaryOption *opt = getDictData();

  AAADiameterHeader &h = *aHeader;
  aBuffer->rd_ptr(aBuffer->base());
  char *p = aBuffer->rd_ptr();
  AAAErrorStatus st;
  AAACommand *com;

  h.ver = *((AAA_UINT8*)(p));
  h.length = ntohl(*((ACE_UINT32*)(p))) & 0x00ffffff;
//  h.length = getlen(p);
  p += 4;
  h.flags.r = *((AAA_UINT8*)(p)) >> 7;
  h.flags.p = *((AAA_UINT8*)(p)) >> 6;
  h.flags.e = *((AAA_UINT8*)(p)) >> 5;
  h.flags.t = *((AAA_UINT8*)(p)) >> 4;

//  h.code = getlen(p);
  h.code = ntohl(*((ACE_UINT32*)(p))) & 0x00ffffff;
  p += 4;
//  h.appId = ntohl32(*((ACE_UINT32*)(p)));
  h.appId = ntohl(*((ACE_UINT32*)(p)));
  p += 4;
  h.hh = ntohl(*((ACE_UINT32*)(p)));
  p += 4;
  h.ee = ntohl(*((ACE_UINT32*)(p)));
  p += 4;

  aBuffer->rd_ptr(p);

  if (opt->option == PARSE_LOOSE)
    {
      return;
    }

  if ((com = AAACommandList::instance()
       ->search(h.code, h.appId, h.flags.r, opt->protocolId)) == NULL)
  {
    AAA_LOG(LM_ERROR, "1.command (%d,r-flag=%1d) not found\n", 
	       h.code, h.flags.r);
    st.set(NORMAL, AAA_COMMAND_UNSUPPORTED);
    throw st;
  }

  if (com->flags.r && h.flags.e)
  {
    AAA_LOG(LM_ERROR, 
	      "command (%d) e flag cannot be set for request\n", 
	      h.code);
    st.set(NORMAL, AAA_INVALID_HDR_BITS);
    throw st;
  }

/*  if (com->flags.p == 0 && h.flags.p == 1)
  {
    AAA_LOG(LM_ERROR, 
	      "command (%d) contains mismatch in p flag\n", 
	      h.code);
    st.set(NORMAL, AAA_INVALID_HDR_BITS);
    throw st;
  }
*/
  /* special handling for answer message with error flag set */
/*  if (! com->flags.r && h.flags.e)
  {
    if (h.flags.p)
    {
      com = AAACommandList::instance()
        ->search("PROXYABLE-ERROR-Answer", opt->protocolId);
    }
    else
    {
      com = AAACommandList::instance()
        ->search("NON-PROXYABLE-ERROR-Answer", opt->protocolId);
    }
    if (com == NULL)
    {
      AAA_LOG(LM_ERROR, 
    		  "3.No way to handle answer with error\n");
      st.set(NORMAL, AAA_COMMAND_UNSUPPORTED);
      throw st;
    }
  }
*/  
  h.dictHandle = com;
}


template<> void
HeaderParserWithProtocol::parseAppToRaw()// throw(AAAErrorStatus)
{
  AAAMessageBlock *aBuffer = getRawData();
  AAADiameterHeader *aHeader = getAppData();
  AAADictionaryOption *opt = getDictData();
  AAADiameterHeader &h = *aHeader;
  char *p = aBuffer->base();
  AAAErrorStatus st;
  AAACommand *com=0;

  aBuffer->wr_ptr(aBuffer->base());

  if (opt->option != PARSE_LOOSE)
  {

    if ((com = AAACommandList::instance()->
	   search(h.code, h.appId, h.flags.r, opt->protocolId)) == NULL)
    {
      AAA_LOG(LM_ERROR, "command (%d,r-flag=%1d) not found\n", 
    		  h.code, h.flags.r);
      st.set(NORMAL, AAA_COMMAND_UNSUPPORTED);
      throw st;
    }

    if (com->flags.r && h.flags.e)
    {
      AAA_LOG(LM_ERROR, 
    		  "command (%d) e flag cannot be set for request\n", 
    		  h.code);
      st.set(NORMAL, AAA_INVALID_HDR_BITS);
      throw st;
    }

/*    if (com->flags.p == 0 && h.flags.p == 1)
    {
      AAA_LOG(LM_ERROR, 
    		  "command (%d) contains mismatch in p flag\n", 
    		  h.code);
      st.set(NORMAL, AAA_INVALID_HDR_BITS);
      throw st;
    }
*/
      /* special handling for answer message with error flag set */
/*    if (! com->flags.r && h.flags.e)
    {
      if (h.flags.p)
      {
        com = AAACommandList::instance()->search("PROXYABLE-ERROR-Answer",
                                                       opt->protocolId);
      }
      else
      {
        com = AAACommandList::instance()->search("NON-PROXYABLE-ERROR-Answer",
                                                       opt->protocolId);
      }
      if (com == NULL)
      {
        AAA_LOG(LM_ERROR, 
    	      "4.No way to handle answer with error\n");
        st.set(NORMAL, AAA_COMMAND_UNSUPPORTED);
        throw st;
      }
    }
*/
  }

  *((ACE_UINT32*)(p)) = ntohl(h.length);
  *((AAA_UINT8*)(p)) = h.ver;
  p += 4;
  *((ACE_UINT32*)(p)) = ntohl(h.code);
  *((AAA_UINT8*)(p)) = 
    (h.flags.r << 7) | (h.flags.p << 6)  | 
    (h.flags.e << 5) | (h.flags.t << 4);;

  p += 4;
  *((ACE_UINT32*)(p)) = ntohl(h.appId);
  p += 4;
  *((ACE_UINT32*)(p)) = ntohl(h.hh);
  p += 4;
  *((ACE_UINT32*)(p)) = ntohl(h.ee);
  p += 4;

  aBuffer->wr_ptr(p);
  h.dictHandle = com;
}
