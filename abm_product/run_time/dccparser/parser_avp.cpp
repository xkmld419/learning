#include <string>
#include "parser.h"
#include "parser_avp.h"
#include "parser_q_avplist.h"
#include "resultcodes.h"
#include "parser_avpvalue.h"

static AvpValueParser*
createAvpValueParser(AAA_AVPDataType type)// throw(AAAErrorStatus)
{
  AAAErrorStatus st;
  AvpType *t = AvpTypeList::instance()->search(type);
  if (t == NULL)
  {
    // Since it has been proven that the implementation recognize
    // the specified type as a result of successful
    // AAADictionary search by qualified AVP list parsre, if
    // the AVP value parser for the corresponding type is not found,
    // it must be the implementation bug.

    AAA_LOG(LM_ERROR, "Specified avp type not found");
    st.set(BUG, MISSING_AVP_DICTIONARY_ENTRY);
    throw (st);
  }

  AvpValueParser *p = t->createParser(type);
  if (p == NULL)
  {
    // This is also a bug for the same reason as above.
    AAA_LOG(LM_ERROR, "Avp value parser not found");
    st.set(BUG, MISSING_AVP_VALUE_PARSER);
    throw (st);
  }

  return p;
}

int
checkFlags(const char *avpName, struct avp_flag flag, AAA_AVPFlag flags)
{
  if (flag.v == 0 && (flags & AAA_AVP_FLAG_VENDOR_SPECIFIC))
  {
    AAA_LOG(LM_ERROR, "[%s] V-flag must be set\n", avpName);
    return -1;
  }
  if (flag.v == 1 && (flags & AAA_AVP_FLAG_VENDOR_SPECIFIC) == 0)
  {
    AAA_LOG(LM_ERROR, "[%s] V-flag must not be set\n", avpName);
    return -1;
  }

  return 0;
}


template<>
void
AvpHeaderParser::parseRawToApp()// throw(AAAErrorStatus)
{
  AvpRawData *rawData = getRawData();
  AAAAvpHeader *h = getAppData();
  AAADictionaryEntry *avp = getDictData();

  AAAAvpHeaderList *ahl = rawData->ahl;
  AAAAvpHeaderList::iterator i;
  AAAErrorStatus st;
  AAAAvpParseType parseType = h->ParseType();

  if (! avp)
  {
	  AAA_LOG(LM_ERROR, "AVP dictionary cannot be null.");
	  st.set(BUG, MISSING_AVP_DICTIONARY_ENTRY);
	  throw st;
  }
  if ( ahl->empty() )
  {
  	
//	  AAA_LOG(LM_ERROR, "ahl is empty.\n");
		this->setErrorStatus(NORMAL, AAA_MISSING_AVP);
//      st.set(NORMAL, AAA_MISSING_AVP, avp);
//      throw st;
    return ;
  }

  if (parseType == PARSE_TYPE_FIXED_HEAD || 
      parseType == PARSE_TYPE_FIXED_TAIL)
  {
    if (avp->avpCode == 0)
	  {
		  AAA_LOG(LM_ERROR, "Wildcard AVP cannot be a fixed AVP.");
//	    setErrorStatus(BUG, INVALID_CONTAINER_PARAM);
		  st.set(BUG, INVALID_CONTAINER_PARAM);
		  throw st;
	  }
      
    for (i=ahl->begin(); i!=ahl->end(); i++) 
    {
//    	i = (parseType == PARSE_TYPE_FIXED_HEAD ? 
//	   ahl->begin() : --ahl->end());

      if ( (*i).code == avp->avpCode)
	    {
	      *h = *i; ahl->erase(i);
	      if (avp->avpCode > 0 && checkFlags(avp->avpName.c_str(), h->flag, avp->flags) != 0)
	      {
//	      st.set(NORMAL, AAA_INVALID_AVP_BITS, avp);
	        st.set(NORMAL, AAA_INVALID_AVP_BIT_COMBO, avp);
	        throw st;
//	      setErrorStatus(NORMAL, AAA_INVALID_AVP_BITS);
          return ;
	      }
	     
	      h->value_p += AVP_HEADER_LEN(avp);  // restore original value_p
	      return;
	    }
    }
  }
  else
  {
    for (i=ahl->begin(); i!=ahl->end(); i++) {
	//ildcard AVPs match any AVPs.
	    if (avp->avpCode != 0)
	    {
	  // For non-wildcard AVPs, strict checking on v-flag, Vencor-Id
	  // and AVP code is performed.
	      if ((*i).flag.v != 
		      ((avp->flags & AAA_AVP_FLAG_VENDOR_SPECIFIC) ? 1 : 0))
	      { continue; }
	      if ((*i).vendor != avp->vendorId)
	      { continue; }
	      if ((*i).code != avp->avpCode) 
	      { continue; }
	    }

	    *h = *i; ahl->erase(i);
	    if (avp->avpCode > 0 && checkFlags(avp->avpName.c_str(), h->flag, avp->flags) != 0)
	    {
//	      st.set(NORMAL, AAA_INVALID_AVP_BITS, avp);
	      st.set(NORMAL, AAA_INVALID_AVP_BIT_COMBO, avp);
	      throw st;
//	      setErrorStatus(NORMAL, AAA_INVALID_AVP_BITS);
        return ;
	    }
	    h->value_p += AVP_HEADER_LEN(avp);  // restore original value_p
	    return;
    }
  }

//  st.set(NORMAL, AAA_MISSING_AVP, avp);
//  throw st;
  this->setErrorStatus(NORMAL, AAA_MISSING_AVP);
  return ;
}

/* a set of functions used for payload construction */
template<>
void
AvpHeaderParser::parseAppToRaw()// throw(AAAErrorStatus)
{
  AvpRawData *rawData = getRawData();
  AAAAvpHeader *h = getAppData();
  AAADictionaryEntry *avp = getDictData();

  AAAMessageBlock *aBuffer = rawData->msg;
  char *p=aBuffer->wr_ptr();
  AAAErrorStatus st;

  /* length check */
  if ((unsigned)AVP_HEADER_LEN(avp) > aBuffer->size())
  {
    AAA_LOG(LM_ERROR, "Header buffer overflow\n");
    st.set(BUG, INVALID_PARSER_USAGE);
    throw st;
  }

  *((ACE_UINT32*)p) = ntohl(avp->avpCode);
//  hl32tobuf(avp->avpCode,p);
  p+=4;
  /* initialize this field to prepare for bit OR operation */
  *((ACE_UINT32*)p) = 0;     
//  *p = 0;          
  if (avp->flags & AAA_AVP_FLAG_VENDOR_SPECIFIC)
  {
    *p|=0x80;
  }
  if (avp->flags & AAA_AVP_FLAG_MANDATORY)
  {
    *p|=0x40;
  }
#if 0
  if (avp->flags & AAA_AVP_FLAG_END_TO_END_ENCRYPT)
  {
    *p|=0x20;
  }
#endif

//  hl32tobuf(h->length,p);
  *((ACE_UINT32*)p) |= ntohl(h->length & 0x00ffffff);
  p+=4;

  if (avp->flags & AAA_AVP_FLAG_VENDOR_SPECIFIC)
  {
    *((ACE_UINT32*)p) = ntohl(avp->vendorId);
//    hl32tobuf(avp->vendorId, p);
    p+=4;
  }
  aBuffer->wr_ptr(p);
}

template<>
void
AvpParser::parseRawToApp()// throw(AAAErrorStatus)
{
#ifdef DEBUG
  struct timeval start_timeval , end_timeval;
  gettimeofday(&start_timeval, NULL);
#endif

  AvpRawData* rawData = getRawData();
  AAAAvpContainer *c = getAppData();
  AAADictionaryEntry *avp = getDictData();
  
  AAAErrorStatus st;
  AAAMessageBlock *aBuffer;
#ifdef DEBUG
  gettimeofday(&end_timeval, NULL);
  std::cout << "########1: AvpParser::init---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
     << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
  gettimeofday(&start_timeval, NULL);
#endif


  for (int i=0; ; i++) 
  {
#ifdef DEBUG
    gettimeofday(&start_timeval, NULL);
#endif
    AAAAvpContainerEntryManager em;
    AvpValueParser *vp;
    AAAAvpContainerEntry* e;
    AAAAvpHeader h;
    h.ParseType() = c->ParseType();
    
    
    /* header check */
    AvpHeaderParser hp;
    hp.setRawData(rawData);
    hp.setAppData(&h);
    hp.setDictData(avp);
    try {
	    hp.parseRawToApp();
    }
    catch (AAAErrorStatus st)
	  {
	    int type, code;
	    st.get(type, code);

	    if (i>0 && type == NORMAL && code == AAA_MISSING_AVP)
	    {
	      // return if no more entry is found once after getting 
	      // at lease one entry received.
#ifdef DEBUG
        gettimeofday(&end_timeval, NULL);
        std::cout << "########1: AvpHeaderParser["<< i << "]---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
	        << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
        gettimeofday(&start_timeval, NULL);
#endif
	      return;
	    }
	    throw st;
	  }

    hp.getErrorStatus(type, code);
    if (type == NORMAL && code == AAA_MISSING_AVP)
    {
#ifdef DEBUG
      gettimeofday(&end_timeval, NULL);
      std::cout << "########2: AvpHeaderParser["<< i << "]---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
	        << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
      gettimeofday(&start_timeval, NULL);
#endif
      if( i == 0  ) 
      {
//        printf("set setErrorStatus\n"); 
        this->setErrorStatus(type, code);
      }
			else 
			{
	      this->setErrorStatus(type, 0);
      }
      return;
    }

    /* payload check */
    e = em.acquire(avp->avpType);
    c->add(e);
#ifdef DEBUG
    gettimeofday(&end_timeval, NULL);
    std::cout << "########: AvpHeaderParser---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
	   << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
    gettimeofday(&start_timeval, NULL);
#endif

    try {
	    vp = createAvpValueParser(avp->avpType);
    }
    catch (AAAErrorStatus st) {
	    throw st;
    }
	
    aBuffer = 
	    AAAMessageBlock::Acquire(h.value_p, h.length-AVP_HEADER_LEN(avp));
    vp->setRawData(aBuffer);
    vp->setAppData(e);
    vp->setDictData(avp);

    try {
	    vp->parseRawToApp();
    }
    catch (AAAErrorStatus st)
    {
      aBuffer->Release();
      delete vp;
      throw st;
    }	
    delete vp;
    aBuffer->Release();
    
    if( getAvpCheck() && avp->avpType == AAA_AVP_ENUM_TYPE ) 
    {
    	int pos;
    	pos = AvpEnumeratedInfo::instance()->avpEnumeratedHashmap[h.code];
      if( pos < 0 ) 
        continue ;
//    printf("EnumAvpCode=%d,data=%d,avp_code=%d,size=%d, v_size=%d\n", h.code, e->dataRef(Type2Type<diameter_unsigned32_t>()), AvpEnumeratedInfo::instance()->v_item[pos].avpCode, AvpEnumeratedInfo::instance()->v_item[pos].vectAvpEnumValue.size(), AvpEnumeratedInfo::instance()->v_item.size());
      int num;
      num = AvpEnumeratedInfo::instance()->v_item[pos].vectAvpEnumValue[0]+AvpEnumeratedInfo::instance()->v_item[pos].vectAvpEnumValue.size();
    	 
    	if( e->dataRef(Type2Type<diameter_unsigned32_t>()) <  AvpEnumeratedInfo::instance()->v_item[pos].vectAvpEnumValue[0] || 
    	    e->dataRef(Type2Type<diameter_unsigned32_t>()) >=  num )
    	{
        AAA_LOG(LM_ERROR, "invalid avp enum value [%d], avp code:[%d] correct enum value in:[%d-%d]\n", e->dataRef(Type2Type<diameter_unsigned32_t>()), h.code,  AvpEnumeratedInfo::instance()->v_item[pos].vectAvpEnumValue[0], num - 1);
        st.set(NORMAL, AAA_INVALID_AVP_VALUE);
        throw st;
    	}
    }
    
#ifdef DEBUG
    gettimeofday(&end_timeval, NULL);
    std::cout << "########: AvpValueParser---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
	   << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
    gettimeofday(&start_timeval, NULL);
#endif
  }
#ifdef DEBUG
  gettimeofday(&end_timeval, NULL);
  std::cout << "########: AvpParser---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
	 << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
  gettimeofday(&start_timeval, NULL);
#endif

}

template<>
void
AvpParser::parseAppToRaw()// throw(AAAErrorStatus)
{
  AvpRawData* rawData = getRawData();
  AAAAvpContainer *c = getAppData();
  AAADictionaryEntry *avp = getDictData();

  AAAMessageBlock *aBuffer = rawData->msg;
  AAAAvpHeader h;
  AAAErrorStatus st;
  if (!avp)
  {
	  AAA_LOG(LM_ERROR, "AVP dictionary cannot be null.");
	  st.set(BUG, MISSING_AVP_DICTIONARY_ENTRY);
	  throw st;
  }
  h.ParseType() = c->ParseType();

  if (avp->avpType == AAA_AVP_DATA_TYPE)  /* Any AVP */
  {
    for (unsigned int i=0; i<c->size(); i++)
	  {
      AvpValueParser *vp;
      try {
        vp = createAvpValueParser(avp->avpType);
      }
      catch (AAAErrorStatus st) {
        throw st;
      }
      vp->setRawData(aBuffer);
      vp->setAppData((*c)[i]);
      vp->setDictData(avp);
      vp->parseAppToRaw();

      aBuffer->wr_ptr
	      (aBuffer->base() + 
	       adjust_word_boundary(aBuffer->wr_ptr() - aBuffer->base()));

	    delete vp;

	  }
    return;
  }

  for (unsigned int i=0; i<c->size(); i++)
  {
    char *saved_p = aBuffer->wr_ptr();
    memset(&h, 0, sizeof(h));
    AvpHeaderParser hp;
    hp.setRawData(rawData);
    hp.setAppData(&h);
    hp.setDictData(avp);
    hp.parseAppToRaw();

//    printf("avp->avpType=%d,avpName=%s\n", avp->avpType, avp->avpName.c_str());
    AvpValueParser *vp = createAvpValueParser(avp->avpType);
    vp->setRawData(aBuffer);
    vp->setAppData((*c)[i]);
    vp->setDictData(avp);
    vp->parseAppToRaw();
    delete vp;
    // calculate the actual header length 
    h.length = ACE_UINT32(aBuffer->wr_ptr() - saved_p);

    // save the current write pointer
    saved_p = aBuffer->base() +
	    adjust_word_boundary(aBuffer->wr_ptr() - aBuffer->base());

      // set the header again
    aBuffer->wr_ptr(aBuffer->wr_ptr() - h.length);
    hp.parseAppToRaw();

    // restore the write pointer
    aBuffer->wr_ptr(saved_p);
  }
}
