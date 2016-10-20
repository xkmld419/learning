#include "avplist.h"
#include "g_avplist.h"
#include "diameter_parser_api.h"

static AAADictionaryEntry Any(0, "AVP", AAA_AVP_DATA_TYPE, 0, 0, 0, 0);

AAADictionaryEntry::AAADictionaryEntry(AAA_AVPCode code,
				       const char* name, 
				       AAA_AVPDataType type, 
				       AAAVendorId vid, 
				       int         sid,
				       AAA_AVPFlag flgs,
                                       int proto)
{
  avpCode = code;
  avpName = name;
  avpType = type;
  vendorId = vid;
  scenarioId = sid;
  flags = flgs;
  protocol = proto;
}

// calculate minimum AVP length
ACE_UINT32
getMinSize(AAADictionaryEntry *avp) throw (AAAErrorStatus) 
{
  AAAGroupedAVP *gavp;
  AvpType *avpt;
  int sum=0;
  AAAErrorStatus st;

  if (!avp)
  {
	  AAA_LOG(LM_ERROR, "getMinSize(): AVP dictionary cannot be null.");
    st.set(BUG, MISSING_AVP_DICTIONARY_ENTRY);
	  throw st;
  }

  avpt = AvpTypeList::instance()->search(avp->avpType);
  if (!avpt)
  {
	  AAA_LOG(LM_ERROR, "getMinSize(): Cannot find AVP type.");
    st.set(BUG,MISSING_AVP_VALUE_PARSER);
	  throw st;
  }

  if (avp->avpType == AAA_AVP_GROUPED_TYPE)
  {
    gavp = AAAGroupedAvpList::instance()->search(avp->avpCode, avp->vendorId, avp->scenarioId);
    if (!gavp)
	  {
		  AAA_LOG(LM_ERROR, "getMinSize(): Cannot grouped AVP dictionary.");
		  st.set(BUG,MISSING_AVP_DICTIONARY_ENTRY);
	    throw st;
    }

      /* Fixed AVPs */
    sum = gavp->avp_f->getMinSize();
      /* Required AVPs */
    sum = gavp->avp_r->getMinSize();
  }
  sum += avpt->getMinSize() + 8 + (avp->vendorId ? 4 : 0);  
  // getMinSize() returns 0 for grouped AVP */
  return sum;
}

AAAAvpList::AAAAvpList() 
{ 
	this->add(&Any); 
  avpCodeHashmap.clear();
	v_item.clear();
}

AAAAvpList::~AAAAvpList()
{
  std::list<AAADictionaryEntry*>::iterator i;
  pop_front();  // remove Any AVP
  for (i=begin(); i!=end(); i++) { delete *i; }
  avpCodeHashmap.clear();
  v_item.clear();

}

void AAAAvpList::release()
{
  std::list<AAADictionaryEntry*>::iterator i;
  pop_front();  // remove Any AVP
  for (i=begin(); i!=end(); i++) { 
  	delete *i; 
  }
  erase(begin(), end());
  avpCodeHashmap.clear();
  v_item.clear();
  this->add(&Any);
  
}

void AAAAvpList::add(AAADictionaryEntry *avp)
{
  if (this->search(avp->avpName, avp->protocol, avp->scenarioId) != NULL)
  {
    AAA_LOG(LM_ERROR, "duplicated AVP definition [%s].\n", avp->avpName.c_str());
    exit(1);
  }
#ifdef DEBUG
  printf("?????????????avp->avpName:[%s]\n", avp->avpName.c_str());
#endif  
// mutex.acquire();
  avpCodeInfo.avpName = avp->avpName;
  avpCodeInfo.avpCode = avp->avpCode;
  push_back(avp);
  
  v_item.push_back(avpCodeInfo);
  //printf("v_item avpCode:[%d], avpName:[%s], size:[%d]\n", avpCodeInfo.avpCode, avpCodeInfo.avpName.c_str(), v_item.size());
  avpCodeHashmap[avpCodeInfo.avpCode] = &v_item[v_item.size()-1];
//  mutex.release();
}

AAADictionaryEntry*
AAAAvpList::search(const std::string& avpName, int protocol, int scenarioId)
{
	
//  mutex.acquire();
  std::list<AAADictionaryEntry*>::iterator i;
  for (i = begin(); i!=end(); i++)
  {
    if ((*i)->avpName == avpName && scenarioId == (*i)->scenarioId )
    {
      if (((*i)->avpCode != 0) &&
          (protocol >= 0) &&
          ((*i)->protocol != protocol)) {
        continue;
      }
//	    mutex.release();
	    return *i;
	  }
  }
//  mutex.release();
  return NULL;
}

AAADictionaryEntry*
AAAAvpList::search(AAA_AVPCode code, AAAVendorId vendor,
                     int protocol, int scenarioId)
{
//  mutex.acquire();
  
  std::list<AAADictionaryEntry*>::iterator i;
  for (i = begin(); i!=end(); i++)
  {
    if( ((*i)->scenarioId == scenarioId) && ((*i)->avpCode == code) && ((*i)->vendorId == vendor) )
	  {
      if (((*i)->avpCode != 0) &&
           (protocol >= 0) &&
           ((*i)->protocol != protocol)) {
        continue;
      }
//	    mutex.release();
	    return *i;
	  }
  }

//  mutex.release();
  return NULL;
}


AvpEnumeratedInfo::AvpEnumeratedInfo() 
{ 
	 v_item.clear();
}

AvpEnumeratedInfo::~AvpEnumeratedInfo()
{
  std::vector<AvpEnumInfo>::iterator i;
 	for ( i = v_item.begin() ; i !=v_item.end(); i++)
  {
    (*i).vectAvpEnumValue.clear();
  } 
  avpEnumeratedHashmap.clear();
  v_item.clear();

}

void AvpEnumeratedInfo::release()
{
  std::vector<AvpEnumInfo>::iterator i;
 	for ( i = v_item.begin() ; i !=v_item.end(); i++)
  {
    (*i).vectAvpEnumValue.clear();
  } 
  avpEnumeratedHashmap.clear();
  v_item.clear();
}

void AvpEnumeratedInfo::add(AAA_AVPCode avpCode, ACE_UINT32 avpEnumValue)
{
  int flag = 0 ;
  //std::vector<AvpEnumInfo>::iterator i;
/* 	for ( i = v_item.begin() ; i !=v_item.end(); i++)
  {
    if( (*i).avpCode == avpCode) 
    {
      flag = 1 ;
      break;
    }
  } 
*/  
  int i ;
 	for ( i = 0 ; i < v_item.size(); i++ )
  {
    if( v_item[i].avpCode == avpCode) 
    {
      flag = 1 ;
      break;
    }
  } 

  if( !flag ) 
  {
    avpEnumInfo.avpCode = avpCode;
    avpEnumInfo.vectAvpEnumValue.clear();
    avpEnumInfo.vectAvpEnumValue.push_back(avpEnumValue);
    v_item.push_back(avpEnumInfo);
    avpEnumeratedHashmap[avpCode] = v_item.size()-1; 
    
//    printf("1.avpCode:[%d],avpEnumValue:[%d],v_size=%d,\n", avpCode, avpEnumValue, v_item.size());
  }
  else 
  {
  	if( avpEnumValue >= v_item[i].vectAvpEnumValue.size() + v_item[i].vectAvpEnumValue[0])
    {
      v_item[i].vectAvpEnumValue.push_back(avpEnumValue);
//      printf("2.avpCode:[%d],avpEnumValue:[%d],v_item[%d].vectAvpEnumValue.size=%d\n", avpCode, avpEnumValue, i, v_item[i].vectAvpEnumValue.size());
    }
  }
}

