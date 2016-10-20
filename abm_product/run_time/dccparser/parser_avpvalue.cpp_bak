#include "parser_avpvalue.h"

// static variables
template<> void AvpValueParser::parseRawToApp() {}// throw (AAAErrorStatus) {}
template<> void AvpValueParser::parseAppToRaw() {}//throw (AAAErrorStatus) {}
static AAAAvpContainerEntry*    pInteger32AvpContainerEntry   = new AAAInteger32AvpContainerEntry(AAA_AVP_INTEGER32_TYPE);
static AAAAvpContainerEntry*    pUnsigned32AvpContainerEntry  = new AAAUnsigned32AvpContainerEntry(AAA_AVP_UINTEGER32_TYPE);
static AAAAvpContainerEntry*    pInteger64AvpContainerEntry   = new AAAInteger64AvpContainerEntry(AAA_AVP_INTEGER64_TYPE);
static AAAAvpContainerEntry*    pUnsigned64AvpContainerEntry  = new AAAUnsigned64AvpContainerEntry(AAA_AVP_UINTEGER64_TYPE);
static AAAAvpContainerEntry*    pStringAvpContainerEntry      = new AAAStringAvpContainerEntry(AAA_AVP_STRING_TYPE);
static AAAAvpContainerEntry*    pAddressAvpContainerEntry     = new AAAAddressAvpContainerEntry(AAA_AVP_ADDRESS_TYPE);
static AAAAvpContainerEntry*    pIPFilterRuleAvpContainerEntry= new AAAIPFilterRuleAvpContainerEntry(AAA_AVP_IPFILTER_RULE_TYPE);
static AAAAvpContainerEntry*    pDiamURIAvpContainerEntry     = new AAADiamURIAvpContainerEntry(AAA_AVP_DIAMURI_TYPE);

void AvpTypeList::registerDefaultTypes()
{
  // Registering default AVP types and AVP value parsers.  This
  // function is called from the constructor.  Since AvpTypeList is
  // used as a singleton, this function is called just one time when
  // it is used at the first time.
  add(new AvpType("Any", AAA_AVP_DATA_TYPE, 0));

  add(new AvpType("Integer32", AAA_AVP_INTEGER32_TYPE, 4));

  add(new AvpType("Integer64", AAA_AVP_INTEGER64_TYPE, 8));

  add(new AvpType("Unsigned32",  AAA_AVP_UINTEGER32_TYPE, 4));

  add(new AvpType("Unsigned64", AAA_AVP_UINTEGER64_TYPE, 8));

  add(new AvpType("UTF8String", AAA_AVP_UTF8_STRING_TYPE, 0));

  add(new AvpType("Enumerated", AAA_AVP_ENUM_TYPE, 4));

  add(new AvpType("OctetString", AAA_AVP_STRING_TYPE, 0));

  add(new AvpType("DiameterIdentity", AAA_AVP_DIAMID_TYPE, 0));

  add(new AvpType("DiameterURI", AAA_AVP_DIAMURI_TYPE, 0));

  add(new AvpType("IPFilterRule", AAA_AVP_IPFILTER_RULE_TYPE, 0));

  add(new AvpType("Grouped", AAA_AVP_GROUPED_TYPE, 0));

  add(new AvpType("Address", AAA_AVP_ADDRESS_TYPE, 0));
  
#if 0
  add(new AvpType("Time", AAA_AVP_TIME_TYPE, 0));
#endif
} 


AAAAvpContainerEntry * AvpType::createContainerEntry(int type)// throw(AAAErrorStatus)
{
  AAAErrorStatus st;
  AAAAvpContainerEntry *e=NULL ; 
  switch(type) 
  {
    case AAA_AVP_INTEGER32_TYPE:
    	e = new AAAInteger32AvpContainerEntry(type);
      break;
    case AAA_AVP_ENUM_TYPE:
    case AAA_AVP_UINTEGER32_TYPE:
    	e = new AAAUnsigned32AvpContainerEntry(type);
      break;
    case AAA_AVP_INTEGER64_TYPE:
    	e = new AAAInteger64AvpContainerEntry(type);
      break;
    case AAA_AVP_UINTEGER64_TYPE:
    	e = new AAAUnsigned64AvpContainerEntry(type);
    	break;
    case AAA_AVP_DATA_TYPE:
    case AAA_AVP_STRING_TYPE:
    case AAA_AVP_DIAMID_TYPE:
    case AAA_AVP_UTF8_STRING_TYPE:
    	e = new AAAStringAvpContainerEntry(type);
      break;
    case AAA_AVP_ADDRESS_TYPE:
    	e = new AAAAddressAvpContainerEntry(type);
      break;
    case AAA_AVP_IPFILTER_RULE_TYPE:
    	e = new AAAIPFilterRuleAvpContainerEntry(type);
      break;
    case AAA_AVP_DIAMURI_TYPE:
    	e = new AAADiamURIAvpContainerEntry(type);
      break; 
    case AAA_AVP_GROUPED_TYPE:
    	e = new AAAGroupedAvpContainerEntry(type);
      break; 
    default:
    	break;
  }
    
  if (e == NULL)
  {
    // This is also a bug for the same reason as above.
    AAA_LOG(LM_ERROR, "Avp value parser not found");
    st.set(BUG, MISSING_AVP_VALUE_PARSER);
    throw (st);
  }

  return e;
} 

AvpValueParser* AvpType::createParser(int type)// throw(AAAErrorStatus)
{
  AAAErrorStatus st;

  AvpValueParser *p=NULL ; 
  switch(type) 
  {
   	case AAA_AVP_DATA_TYPE:
    	p = new AnyParser;
      break;
    case AAA_AVP_UINTEGER32_TYPE:
    case AAA_AVP_INTEGER32_TYPE:
    case AAA_AVP_ENUM_TYPE:
    	p = new Integer32Parser;
    	break;
    case AAA_AVP_UINTEGER64_TYPE:
    case AAA_AVP_INTEGER64_TYPE:
    	p = new Integer64Parser;
    	break;
    case AAA_AVP_STRING_TYPE:
    case AAA_AVP_DIAMID_TYPE:
    	p = new OctetstringParser;
      break;
    case AAA_AVP_UTF8_STRING_TYPE:
    	p = new Utf8stringParser;
      break;
    case AAA_AVP_ADDRESS_TYPE:
    	p = new AddressParser;
      break;
    case AAA_AVP_IPFILTER_RULE_TYPE:
    	p = new IPFilterRuleParser;
      break;
    case AAA_AVP_GROUPED_TYPE:
    	p = new GroupedParser;
      break;
    case AAA_AVP_DIAMURI_TYPE:
    	p = new DiamuriParser;
      break; 
    default:
    	break;
  }
    
  if (p == NULL)
  {
    // This is also a bug for the same reason as above.
    AAA_LOG(LM_ERROR, "Avp value parser not found");
    st.set(BUG, MISSING_AVP_VALUE_PARSER);
    throw (st);
  }

  return p;
}
