/* BEGIN_COPYRIGHT                                                        */
/*                                                                        */
/* Open Diameter: Open-source software for the Diameter and               */
/*                Diameter related protocols                              */
/*                                                                        */
/* Copyright (C) 2002-2004 Open Diameter Project                          */
/*                                                                        */
/* This library is free software; you can redistribute it and/or modify   */
/* it under the terms of the GNU Lesser General Public License as         */
/* published by the Free Software Foundation; either version 2.1 of the   */
/* License, or (at your option) any later version.                        */
/*                                                                        */
/* This library is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      */
/* Lesser General Public License for more details.                        */
/*                                                                        */
/* You should have received a copy of the GNU Lesser General Public       */
/* License along with this library; if not, write to the Free Software    */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307    */
/* USA.                                                                   */
/*                                                                        */
/* In addition, when you copy and redistribute some or the entire part of */
/* the source code of this software with or without modification, you     */
/* MUST include this copyright notice in each copy.                       */
/*                                                                        */
/* If you make any changes that are appeared to be useful, please send    */
/* sources that include the changed part to                               */
/* diameter-developers@lists.sourceforge.net so that we can reflect your  */
/* changes to one unified version of this software.                       */
/*                                                                        */
/* END_COPYRIGHT                                                          */
/* $Id: parser_avpvalue.cxx,v 1.17 2004/06/17 21:07:49 yohba Exp $ */

#include "parser_avpvalue.h"

// static variables
template<> void AvpValueParser::parseRawToApp() {}// throw (AAAErrorStatus) {}
template<> void AvpValueParser::parseAppToRaw() {}//throw (AAAErrorStatus) {}

/* 
/// Container entry creators.
static AvpContainerEntryCreator<AAAStringAvpContainerEntry> 
stringContainerEntryCreator;
static AvpContainerEntryCreator<AAAInteger32AvpContainerEntry> 
integer32ContainerEntryCreator;
static AvpContainerEntryCreator<AAAUnsigned32AvpContainerEntry> 
unsigned32ContainerEntryCreator;
static AvpContainerEntryCreator<AAAInteger64AvpContainerEntry> 
integer64ContainerEntryCreator;
static AvpContainerEntryCreator<AAAUnsigned64AvpContainerEntry> 
unsigned64ContainerEntryCreator;
static AvpContainerEntryCreator<AAADiamURIAvpContainerEntry> 
diamuriContainerEntryCreator;
static AvpContainerEntryCreator<AAAIPFilterRuleAvpContainerEntry> 
ipfilterRuleContainerEntryCreator;
static AvpContainerEntryCreator<AAAGroupedAvpContainerEntry> 
groupedContainerEntryCreator;
static AvpContainerEntryCreator<AAAAddressAvpContainerEntry> 
addressContainerEntryCreator;
*/

/* static stringContainerEntryCreator       AvpStringContainerEntryCreator;
static integer32ContainerEntryCreator    AvpInteger32ContainerEntryCreator;
static unsigned32ContainerEntryCreator   AvpUnsigned32ContainerEntryCreator;
static integer64ContainerEntryCreator    AvpInteger64ContainerEntryCreator;
static unsigned64ContainerEntryCreator   AvpUnsigned64ContainerEntryCreator;
static diamuriContainerEntryCreator      AvpDiamuriContainerEntryCreator; 
static ipfilterRuleContainerEntryCreator AvpIpfilterRuleContainerEntryCreator;
static groupedContainerEntryCreator      AvpGroupedContainerEntryCreator;
static addressContainerEntryCreator      AvpAddressContainerEntryCreator;


static AAAInteger32AvpContainerEntry*    pInteger32AvpContainerEntry   = new AAAInteger32AvpContainerEntry(AAA_AVP_INTEGER32_TYPE);
static AAAUnsigned32AvpContainerEntry*   pUnsigned32AvpContainerEntry  = new AAAUnsigned32AvpContainerEntry(AAA_AVP_UINTEGER32_TYPE);
static AAAInteger64AvpContainerEntry*    pInteger64AvpContainerEntry   = new AAAInteger64AvpContainerEntry(AAA_AVP_INTEGER64_TYPE);
static AAAUnsigned64AvpContainerEntry*   pUnsigned64AvpContainerEntry  = new AAAUnsigned64AvpContainerEntry(AAA_AVP_UINTEGER64_TYPE);
static AAAStringAvpContainerEntry*       pStringAvpContainerEntry      = new AAAStringAvpContainerEntry(AAA_AVP_STRING_TYPE);
static AAAAddressAvpContainerEntry*      pAddressAvpContainerEntry     = new AAAAddressAvpContainerEntry(AAA_AVP_ADDRESS_TYPE);
static AAAIPFilterRuleAvpContainerEntry* pIPFilterRuleAvpContainerEntry= new AAAIPFilterRuleAvpContainerEntry(AAA_AVP_IPFILTER_RULE_TYPE);
static AAADiamURIAvpContainerEntry*      pDiamURIAvpContainerEntry     = new AAADiamURIAvpContainerEntry(AAA_AVP_DIAMURI_TYPE);
static AAAGroupedAvpContainerEntry*      pGroupedAvpContainerEntry     = new AAAGroupedAvpContainerEntry(AAA_AVP_GROUPED_TYPE);

*/
static AAAAvpContainerEntry*    pInteger32AvpContainerEntry   = new AAAInteger32AvpContainerEntry(AAA_AVP_INTEGER32_TYPE);
static AAAAvpContainerEntry*    pUnsigned32AvpContainerEntry  = new AAAUnsigned32AvpContainerEntry(AAA_AVP_UINTEGER32_TYPE);
static AAAAvpContainerEntry*    pInteger64AvpContainerEntry   = new AAAInteger64AvpContainerEntry(AAA_AVP_INTEGER64_TYPE);
static AAAAvpContainerEntry*    pUnsigned64AvpContainerEntry  = new AAAUnsigned64AvpContainerEntry(AAA_AVP_UINTEGER64_TYPE);
static AAAAvpContainerEntry*    pStringAvpContainerEntry      = new AAAStringAvpContainerEntry(AAA_AVP_STRING_TYPE);
static AAAAvpContainerEntry*    pAddressAvpContainerEntry     = new AAAAddressAvpContainerEntry(AAA_AVP_ADDRESS_TYPE);
static AAAAvpContainerEntry*    pIPFilterRuleAvpContainerEntry= new AAAIPFilterRuleAvpContainerEntry(AAA_AVP_IPFILTER_RULE_TYPE);
static AAAAvpContainerEntry*    pDiamURIAvpContainerEntry     = new AAADiamURIAvpContainerEntry(AAA_AVP_DIAMURI_TYPE);
//static AAAAvpContainerEntry*    pGroupedAvpContainerEntry     = new AAAGroupedAvpContainerEntry(AAA_AVP_GROUPED_TYPE);
//static AAAAvpContainerEntry*    pGroupedAvpContainerEntry;

AnyParser* _avp_parse_type::p_AnyParser = new AnyParser;
Integer32Parser* _avp_parse_type::p_Integer32Parser = new Integer32Parser;
Integer64Parser* _avp_parse_type::p_Integer64Parser = new Integer64Parser;
OctetstringParser* _avp_parse_type::p_OctetstringParser = new OctetstringParser;
Utf8stringParser* _avp_parse_type::p_Utf8stringParser = new Utf8stringParser;
AddressParser* _avp_parse_type::p_AddressParser = new AddressParser;
IPFilterRuleParser* _avp_parse_type::p_IPFilterRuleParser = new IPFilterRuleParser;
GroupedParser* _avp_parse_type::p_GroupedParser = new GroupedParser;
DiamuriParser* _avp_parse_type::p_DiamuriParser = new DiamuriParser;

/* 
void AvpTypeList::registerDefaultTypes()
{
  // Registering default AVP types and AVP value parsers.  This
  // function is called from the constructor.  Since AvpTypeList is
  // used as a singleton, this function is called just one time when
  // it is used at the first time.
  add(new AvpType("Any", AAA_AVP_DATA_TYPE, 0, pStringAvpContainerEntry));

  add(new AvpType("Integer32", AAA_AVP_INTEGER32_TYPE, 4, pInteger32AvpContainerEntry));

  add(new AvpType("Integer64", AAA_AVP_INTEGER64_TYPE, 8, pInteger64AvpContainerEntry));

  add(new AvpType("Unsigned32",  AAA_AVP_UINTEGER32_TYPE, 4, pUnsigned32AvpContainerEntry));

  add(new AvpType("Unsigned64", AAA_AVP_UINTEGER64_TYPE, 8, pUnsigned64AvpContainerEntry));

  add(new AvpType("UTF8String", AAA_AVP_UTF8_STRING_TYPE, 0, pStringAvpContainerEntry));

  add(new AvpType("Enumerated", AAA_AVP_ENUM_TYPE, 4, pUnsigned32AvpContainerEntry));

  add(new AvpType("OctetString", AAA_AVP_STRING_TYPE, 0, pStringAvpContainerEntry));

  add(new AvpType("DiameterIdentity", AAA_AVP_DIAMID_TYPE, 0, pStringAvpContainerEntry));

  add(new AvpType("DiameterURI", AAA_AVP_DIAMURI_TYPE, 0, pDiamURIAvpContainerEntry));

  add(new AvpType("IPFilterRule", AAA_AVP_IPFILTER_RULE_TYPE, 0, pIPFilterRuleAvpContainerEntry));

  add(new AvpType("Grouped", AAA_AVP_GROUPED_TYPE, 0, pGroupedAvpContainerEntry));

  add(new AvpType("Address", AAA_AVP_ADDRESS_TYPE, 0, pAddressAvpContainerEntry));
  
#if 0
  add(new AvpType("Time", AAA_AVP_TIME_TYPE, 0));
#endif
} */

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

/* AAAAvpContainerEntry * AvpType::createContainerEntry(int type)// throw(AAAErrorStatus)
{
	return avpContainerEntry;
}*/

 
/* AAAAvpContainerEntry * AvpType::createContainerEntry(int type)// throw(AAAErrorStatus)
{
  AAAErrorStatus st;


  AAAAvpContainerEntry *e=NULL ; 
  switch(type) 
  {
    case AAA_AVP_INTEGER32_TYPE:
    	if( pInteger32AvpContainerEntry == 0 ) 
    	  pInteger32AvpContainerEntry = new AAAInteger32AvpContainerEntry(AAA_AVP_INTEGER32_TYPE);       
      e = pInteger32AvpContainerEntry ;
      break;
    case AAA_AVP_UINTEGER64_TYPE:
    	if( pUnsigned64AvpContainerEntry == 0 ) 
        pUnsigned64AvpContainerEntry  = new AAAUnsigned64AvpContainerEntry(AAA_AVP_UINTEGER64_TYPE);
     	e = pUnsigned64AvpContainerEntry;
    	break;
    case AAA_AVP_ENUM_TYPE:
    case AAA_AVP_UINTEGER32_TYPE:
      if( pUnsigned32AvpContainerEntry == 0 ) 
        pUnsigned32AvpContainerEntry  = new AAAUnsigned32AvpContainerEntry(AAA_AVP_UINTEGER32_TYPE);
     	e = pUnsigned32AvpContainerEntry;
      break;
    case AAA_AVP_INTEGER64_TYPE:
    	if( pInteger64AvpContainerEntry == 0 ) 
        pInteger64AvpContainerEntry  = new AAAInteger64AvpContainerEntry(AAA_AVP_INTEGER64_TYPE);
     	e = pInteger64AvpContainerEntry;
    	break;
    case AAA_AVP_DATA_TYPE:
    case AAA_AVP_STRING_TYPE:
    case AAA_AVP_DIAMID_TYPE:
    case AAA_AVP_UTF8_STRING_TYPE:
    	if( pStringAvpContainerEntry == 0 ) 
    	  pStringAvpContainerEntry = new AAAStringAvpContainerEntry(AAA_AVP_STRING_TYPE);
    	e = pStringAvpContainerEntry;
      break;
    case AAA_AVP_ADDRESS_TYPE:
    	if( pAddressAvpContainerEntry == 0 ) 
        pAddressAvpContainerEntry     = new AAAAddressAvpContainerEntry(AAA_AVP_ADDRESS_TYPE);
    	e = pAddressAvpContainerEntry;
      break;
    case AAA_AVP_IPFILTER_RULE_TYPE:
    	if( pIPFilterRuleAvpContainerEntry == 0 ) 
        pIPFilterRuleAvpContainerEntry= new AAAIPFilterRuleAvpContainerEntry(AAA_AVP_IPFILTER_RULE_TYPE);
    	e = pIPFilterRuleAvpContainerEntry;
      break;
    case AAA_AVP_DIAMURI_TYPE:
      if( pDiamURIAvpContainerEntry == 0 )     
        pDiamURIAvpContainerEntry     = new AAADiamURIAvpContainerEntry(AAA_AVP_DIAMURI_TYPE);
	    e = pDiamURIAvpContainerEntry;
      break; 
    case AAA_AVP_GROUPED_TYPE:
      if( pGroupedAvpContainerEntry == 0 ) 
        pGroupedAvpContainerEntry = new AAAGroupedAvpContainerEntry(AAA_AVP_GROUPED_TYPE);
    	e = pGroupedAvpContainerEntry;
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
} */ 



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

/*

AAAAvpContainerEntry * AvpType::createContainerEntry(int type)// throw(AAAErrorStatus)
{
  AAAErrorStatus st;

  AAAAvpContainerEntry *e=NULL ; 
  switch(type) 
  {
    case AAA_AVP_INTEGER32_TYPE:
    	e = AvpInteger32ContainerEntryCreator(type);
      break;
    case AAA_AVP_ENUM_TYPE:
    case AAA_AVP_UINTEGER32_TYPE:
    	e = AvpUnsigned32ContainerEntryCreator(type);
      break;
    case AAA_AVP_INTEGER64_TYPE:
    	e = AvpInteger64ContainerEntryCreator(type);
      break;
    case AAA_AVP_UINTEGER64_TYPE:
    	e = AvpUnsigned64ContainerEntryCreator(type);
    	break;
    case AAA_AVP_DATA_TYPE:
    case AAA_AVP_STRING_TYPE:
    case AAA_AVP_DIAMID_TYPE:
    case AAA_AVP_UTF8_STRING_TYPE:
    	e = AvpStringContainerEntryCreator(type);
      break;
    case AAA_AVP_ADDRESS_TYPE:
    	e = AvpAddressContainerEntryCreator(type);
      break;
    case AAA_AVP_IPFILTER_RULE_TYPE:
    	e = AvpIpfilterRuleContainerEntryCreator(type);
      break;
    case AAA_AVP_GROUPED_TYPE:
    	e = AvpGroupedContainerEntryCreator(type);
      break; 
    case AAA_AVP_DIAMURI_TYPE:
    	e = AvpDiamuriContainerEntryCreator(type);
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
} */

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
