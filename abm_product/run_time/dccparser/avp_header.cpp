#include "avp_header.h"
#include "parser.h"
#include "avplist.h"

static void
readAvpHeader(char *buf, ACE_UINT32 len, AAAAvpHeader *h)
  throw(AAAErrorStatus)
{
  char *p=buf;

  memset(h, 0, sizeof(*h));

  h->code = ntohl(*((ACE_UINT32*)p)); p+=4;
//
  
  h->flag.v = (*p & 0x80) ? 1 : 0;
  h->flag.m = (*p & 0x40) ? 1 : 0;
  h->flag.p = (*p & 0x20) ? 1 : 0;

  if ( ( (*p&0xff) & 0x1f ) )
  {
    AAAErrorStatus st;
    AAA_LOG(LM_ERROR, "invalid avp code[%d] bits:[%02X]\n", h->code, (*p&0xff) );
    st.set(NORMAL, AAA_INVALID_AVP_BITS);
    throw st;
  }

  h->length = ntohl(*((ACE_UINT32*)p)) & 0x00ffffff; p+=4;
  if (h->length == 0 || h->length > len)
  {
    AAAErrorStatus st;
    AAA_LOG(LM_ERROR, "invalid avp[%d] length:[%d] ,len=%d\n", h->code, h->length, len);
//    st.set(NORMAL, AAA_INVALID_MESSAGE_LENGTH);
    st.set(NORMAL, AAA_INVALID_AVP_LENGTH);
    throw st;
  }


  if (h->flag.v == 1)
  {
    h->vendor = ntohl(*((ACE_UINT32*)p)); p+=4;
  }

//  AAA_LOG(LM_ERROR, "invalid avp code:[%d]\n", h->code);

  if( getAvpCheck() && h->flag.m == 1 && AAAAvpList::instance()->avpCodeHashmap.find(h->code ) == AAAAvpList::instance()->avpCodeHashmap.end()) 
  {
    AAAErrorStatus st;
    AAA_LOG(LM_ERROR, "invalid avp code:[%d]\n", h->code);
    st.set(NORMAL, AAA_AVP_UNSUPPORTED);
    throw st;
  }

  h->value_p = p;
}

/// Create a list of AVP headers.
void
AAAAvpHeaderList::create(AAAMessageBlock *aBuffer)
  throw(AAAErrorStatus)
{
  AAAAvpHeader h;
  char *start = aBuffer->rd_ptr();
  char *end = aBuffer->base()+aBuffer->size();

  for (char *p=start; p<end; p+=adjust_word_boundary(h.length))
  {
    readAvpHeader(p, end - p, &h);
    h.value_p = p;      // Store the pointer to the header head
    push_back(h);  
  }

  aBuffer->rd_ptr(end);
}

