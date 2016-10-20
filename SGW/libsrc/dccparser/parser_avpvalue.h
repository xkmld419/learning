#ifndef __PARSER_AVPVALUE_H__
#define __PARSER_AVPVALUE_H__

#include "parser.h"
#include "parser_avp.h"
#include "resultcodes.h"
#include "parser_q_avplist.h"
#include "q_avplist.h"
#include "g_avplist.h"
#include "diameter_parser_api.h"
#include <iostream>

/// Container entry type definitions.
typedef AAATypeSpecificAvpContainerEntry<diameter_integer32_t>
AAAInteger32AvpContainerEntry;
typedef AAATypeSpecificAvpContainerEntry<diameter_unsigned32_t>
AAAUnsigned32AvpContainerEntry;
typedef AAATypeSpecificAvpContainerEntry<diameter_integer64_t>
AAAInteger64AvpContainerEntry;
typedef AAATypeSpecificAvpContainerEntry<diameter_unsigned64_t>
AAAUnsigned64AvpContainerEntry;
typedef AAATypeSpecificAvpContainerEntry<AAAAvpContainerList>
AAAGroupedAvpContainerEntry;
typedef AAATypeSpecificAvpContainerEntry<std::string>
AAAStringAvpContainerEntry;
typedef AAATypeSpecificAvpContainerEntry<diameter_uri_t>
AAADiamURIAvpContainerEntry;
typedef AAATypeSpecificAvpContainerEntry<diameter_ipfilter_rule_t>
AAAIPFilterRuleAvpContainerEntry;
typedef AAATypeSpecificAvpContainerEntry<diameter_address_t>
AAAAddressAvpContainerEntry;



/// Container entry creators.
typedef AvpContainerEntryCreator<AAAStringAvpContainerEntry>
stringContainerEntryCreator;
typedef AvpContainerEntryCreator<AAAInteger32AvpContainerEntry>
integer32ContainerEntryCreator;
typedef AvpContainerEntryCreator<AAAUnsigned32AvpContainerEntry>
unsigned32ContainerEntryCreator;
typedef AvpContainerEntryCreator<AAAInteger64AvpContainerEntry>
integer64ContainerEntryCreator;
typedef AvpContainerEntryCreator<AAAUnsigned64AvpContainerEntry>
unsigned64ContainerEntryCreator;
typedef AvpContainerEntryCreator<AAADiamURIAvpContainerEntry>
diamuriContainerEntryCreator;
typedef AvpContainerEntryCreator<AAAIPFilterRuleAvpContainerEntry>
ipfilterRuleContainerEntryCreator;
typedef AvpContainerEntryCreator<AAAGroupedAvpContainerEntry>
groupedContainerEntryCreator;
typedef AvpContainerEntryCreator<AAAAddressAvpContainerEntry>
addressContainerEntryCreator;

class AnyParser : public AvpValueParser
{
 public:
  void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAMessageBlock* aBuffer = getRawData();
      AAAStringAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_DATA_TYPE)
	    {
	      AAAErrorStatus st;
	      AAA_LOG(LM_ERROR, "Invalid AVP type.");
	      st.set(BUG, PROHIBITED_CONTAINER);
	      throw st;
	    }
      e->dataRef().assign(aBuffer->base(), aBuffer->size());
    }

    void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAMessageBlock* aBuffer = getRawData();
      AAAStringAvpContainerEntry *e;
      getAppData(e);
      std::string& str = e->dataRef();
      if (e->dataType() != AAA_AVP_DATA_TYPE)
	    {
	      AAAErrorStatus st;
	      AAA_LOG(LM_ERROR, "Invalid AVP type.");
	      st.set(BUG, PROHIBITED_CONTAINER);
	      throw st;
	    }
      if ((aBuffer->size() + (size_t)aBuffer->base() -
           (size_t)aBuffer->wr_ptr()) < str.length())
	/* assuming 32-bit boundary */
	   {
	     AAAErrorStatus st;
	     AAA_LOG(LM_ERROR, "Not enough buffer");
	     st.set(NORMAL, AAA_OUT_OF_SPACE);
	     throw st;
	   }
      aBuffer->copy(str.data(), str.length());
   }
};

class Integer32Parser : public AvpValueParser
{
//  friend class Singleton<Integer32Parser, Null_Mutex>;
 public:
  void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAMessageBlock* aBuffer = getRawData();
      AAAInteger32AvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_INTEGER32_TYPE &&
	  e->dataType() != AAA_AVP_UINTEGER32_TYPE &&
	  e->dataType() != AAA_AVP_ENUM_TYPE)
	{
	  AAAErrorStatus st;
	  AAA_LOG(LM_ERROR, "1.Invalid AVP type.");
	  st.set(BUG, PROHIBITED_CONTAINER);
	  throw st;
	}
      e->dataRef() = ntohl(*((diameter_integer32_t*)(aBuffer->base())));
    }
  void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAMessageBlock* aBuffer;
      AAAInteger32AvpContainerEntry *e;
      getRawData(aBuffer);
      getAppData(e);
      if (e->dataType() != AAA_AVP_INTEGER32_TYPE &&
	  e->dataType() != AAA_AVP_UINTEGER32_TYPE &&
	  e->dataType() != AAA_AVP_ENUM_TYPE)
	{
	  AAAErrorStatus st;
	  AAA_LOG(LM_ERROR, "2.Invalid AVP type:[%d].", e->dataType());
	  st.set(BUG, PROHIBITED_CONTAINER);
	  throw st;
	}
  if ((aBuffer->size() + (size_t)aBuffer->base() -
      (size_t)aBuffer->wr_ptr()) < sizeof(diameter_integer32_t))
	/* assuming 32-bit boundary */
	{
	  AAAErrorStatus st;
	  AAA_LOG(LM_ERROR, "Not enough buffer");
	  st.set(NORMAL, AAA_OUT_OF_SPACE);
	  throw st;
	}
      *((diameter_integer32_t*)(aBuffer->wr_ptr())) = ntohl(e->dataRef());
      aBuffer->wr_ptr(sizeof(diameter_integer32_t));
    }
};


static inline ACE_UINT64
ntohq(ACE_UINT64 q)
{
#if defined(OS_WIN32)||defined(OS_CYGWIN)||defined(OS_LINUX)
  ACE_UINT64 int64;
  char buf1[9], buf2[9];
  memcpy(buf2, &q, 8);
  int i ;
  for( i = 0 ; i < 8 ; i ++ )
  {
  	buf1[i] = buf2[7-i];
  }
  memcpy(&int64, buf1, 8);
  return int64;
#else
    return q;
#endif
}

class Integer64Parser : public AvpValueParser
{
//  friend class Singleton<Integer64Parser, Null_Mutex>;
  public:
    void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAMessageBlock* aBuffer = getRawData();
      AAAInteger64AvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_INTEGER64_TYPE &&
         e->dataType() != AAA_AVP_UINTEGER64_TYPE)
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "3.Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      //modify by panlq 2008-11-02
      //e->dataRef() = ntohq(*((diameter_integer64_t*)(aBuffer->base())));
      diameter_integer64_t int64_tmp;
      memcpy(&int64_tmp,aBuffer->base(),sizeof(int64_tmp));
      e->dataRef() = ntohq(int64_tmp);
    }
    void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAMessageBlock* aBuffer;
      AAAInteger64AvpContainerEntry *e;
      getRawData(aBuffer);
      getAppData(e);
      if (e->dataType() != AAA_AVP_INTEGER64_TYPE &&
          e->dataType() != AAA_AVP_UINTEGER64_TYPE)
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "4.Invalid AVP type:[%d].", e->dataType());
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      if ((aBuffer->size() + (size_t)aBuffer->base() -
               (size_t)aBuffer->wr_ptr()) < sizeof(diameter_integer64_t))
      /* assuming 32-bit boundary */
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Not enough buffer");
        st.set(NORMAL, AAA_OUT_OF_SPACE);
        throw st;
      }
      //modify by panlq 2008-11-02
      //*((diameter_integer64_t*)(aBuffer->wr_ptr())) = ntohq(e->dataRef());
      diameter_integer64_t int64 = ntohq(e->dataRef());
      memcpy(((diameter_integer64_t*)(aBuffer->wr_ptr())),&int64,sizeof(int64));
      aBuffer->wr_ptr(sizeof(diameter_integer64_t));
    }
};

class OctetstringParser : public AvpValueParser
{
  public:
    void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAMessageBlock* aBuffer = getRawData();
      AAAStringAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_UTF8_STRING_TYPE
      	  && e->dataType() != AAA_AVP_STRING_TYPE
      	  && e->dataType() != AAA_AVP_ADDRESS_TYPE
      	  && e->dataType() != AAA_AVP_DIAMID_TYPE )
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      std::string &str = e->dataRef();
      str.assign(aBuffer->base(), aBuffer->size());
    }
    void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAMessageBlock* aBuffer = getRawData();
      AAAStringAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_UTF8_STRING_TYPE
      	  && e->dataType() != AAA_AVP_STRING_TYPE
      	  && e->dataType() != AAA_AVP_ADDRESS_TYPE
      	  && e->dataType() != AAA_AVP_DIAMID_TYPE )
	    {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
	    }
      std::string &str = e->dataRef();

      AAAErrorStatus st;
      if (aBuffer->size() + (size_t)aBuffer->base() -
          (size_t)aBuffer->wr_ptr() < str.length())
      {
        AAA_LOG(LM_ERROR, "Not enough buffer\n");
        st.set(NORMAL, AAA_OUT_OF_SPACE);
        throw st;
      }
      aBuffer->copy(str.data(), str.length());
    }
};


/* UTF-8 definition in RFC 2279.

 2.  UTF-8 definition

   In UTF-8, characters are encoded using sequences of 1 to 6 octets.
   The only octet of a "sequence" of one has the higher-order bit set to
   0, the remaining 7 bits being used to encode the character value. In
   a sequence of n octets, n>1, the initial octet has the n higher-order
   bits set to 1, followed by a bit set to 0.  The remaining bit(s) of
   that octet contain bits from the value of the character to be
   encoded.  The following octet(s) all have the higher-order bit set to
   1 and the following bit set to 0, leaving 6 bits in each to contain
   bits from the character to be encoded.

   The table below summarizes the format of these different octet types.
   The letter x indicates bits available for encoding bits of the UCS-4
   character value.

   UCS-4 range (hex.)           UTF-8 octet sequence (binary)
   0000 0000-0000 007F   0xxxxxxx
   0000 0080-0000 07FF   110xxxxx 10xxxxxx
   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
   0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx

   Encoding from UCS-4 to UTF-8 proceeds as follows:

   1) Determine the number of octets required from the character value
      and the first column of the table above.  It is important to note
      that the rows of the table are mutually exclusive, i.e. there is
      only one valid way to encode a given UCS-4 character.

   2) Prepare the high-order bits of the octets as per the second column
      of the table.

   3) Fill in the bits marked x from the bits of the character value,
      starting from the lower-order bits of the character value and
      putting them first in the last octet of the sequence, then the
      next to last, etc. until all x bits are filled in.

      The algorithm for encoding UCS-2 (or Unicode) to UTF-8 can be
      obtained from the above, in principle, by simply extending each
      UCS-2 character with two zero-valued octets.  However, pairs of
      UCS-2 values between D800 and DFFF (surrogate pairs in Unicode
      parlance), being actually UCS-4 characters transformed through
      UTF-16, need special treatment: the UTF-16 transformation must be
      undone, yielding a UCS-4 character that is then transformed as
      above.

      Decoding from UTF-8 to UCS-4 proceeds as follows:

   1) Initialize the 4 octets of the UCS-4 character with all bits set
      to 0.

   2) Determine which bits encode the character value from the number of
      octets in the sequence and the second column of the table above
      (the bits marked x).

   3) Distribute the bits from the sequence to the UCS-4 character,
      first the lower-order bits from the last octet of the sequence and
      proceeding to the left until no x bits are left.

      If the UTF-8 sequence is no more than three octets long, decoding
      can proceed directly to UCS-2.

        NOTE -- actual implementations of the decoding algorithm above
        should protect against decoding invalid sequences.  For
        instance, a naive implementation may (wrongly) decode the
        invalid UTF-8 sequence C0 80 into the character U+0000, which
        may have security consequences and/or cause other problems.  See
        the Security Considerations section below.

   A more detailed algorithm and formulae can be found in [FSS_UTF],
   [UNICODE] or Annex R to [ISO-10646].

*/

class Utf8stringParser : public OctetstringParser
{
  public:

    void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAStringAvpContainerEntry *e;
      getAppData(e);
      AAADictionaryEntry *avp = getDictData();
         if (e->dataType() != AAA_AVP_UTF8_STRING_TYPE )
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      std::string &str = e->dataRef();

      AAAErrorStatus st;
      e->dataType() = AAA_AVP_STRING_TYPE;
      try {
        OctetstringParser::parseRawToApp();
      }
      catch (AAAErrorStatus st)	{
       	e->dataType() = AAA_AVP_UTF8_STRING_TYPE;
	      throw st;
      }
      e->dataType() = AAA_AVP_UTF8_STRING_TYPE;
      UTF8Checker check;
      if (check(str.data(), str.size()) != 0)
		  {
			  AAA_LOG(LM_ERROR, "Invalid UTF8 string");
			  st.set(NORMAL, AAA_INVALID_AVP_VALUE, avp);
			  throw st;
		  }
    }

    void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAStringAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_UTF8_STRING_TYPE
      	  && e->dataType() != AAA_AVP_STRING_TYPE
      	  && e->dataType() != AAA_AVP_ADDRESS_TYPE
      	  && e->dataType() != AAA_AVP_DIAMID_TYPE )
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      std::string &str = e->dataRef();

      AAAErrorStatus st;
      UTF8Checker check;
      if (check(str.data(), str.size()) != 0)
      {
        AAA_LOG(LM_ERROR, "Invalid UTF8 string");
        st.set(BUG, INVALID_CONTAINER_CONTENTS);
        throw st;
      }
      e->dataType() = AAA_AVP_STRING_TYPE;
      try {
      	OctetstringParser::parseAppToRaw();
      }
      catch (AAAErrorStatus st)	{
	      e->dataType() = AAA_AVP_UTF8_STRING_TYPE;
	      throw st;
      }
      e->dataType() = AAA_AVP_UTF8_STRING_TYPE;
    }
};

class AddressParser : public OctetstringParser
{
  public:
    void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAAvpContainerEntryManager em;
      AAAAddressAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_ADDRESS_TYPE)
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      AAAAvpContainerEntry *e2 = em.acquire(AAA_AVP_STRING_TYPE);
      setAppData(e2);    // Replace the original container entry with
			 // the new address container entry.

      AAAErrorStatus st;
      try {
       	OctetstringParser::parseRawToApp();
      }
      catch (AAAErrorStatus st)
      {
        setAppData(e); // Recover the original container entry.
        em.release(e2);// Release the UTF8 string container entry.
        throw st;
      }
      setAppData(e);     // Recover the original container entry.

      std::string& str = e2->dataRef(Type2Type<diameter_octetstring_t>());
      diameter_address_t &address = e->dataRef();

      if (str.length()>0)
    	  address.type = ntohs(*((ACE_UINT16*)(str.data())));
      if (str.length()>1)
    	  address.value.assign(str, 2, str.length() - 2);

      em.release(e2);    // Release the UTF8 string container entry.
    };
    void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAAvpContainerEntryManager em;
      AAAAddressAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_UTF8_STRING_TYPE
      	  && e->dataType() != AAA_AVP_STRING_TYPE
      	  && e->dataType() != AAA_AVP_ADDRESS_TYPE
      	  && e->dataType() != AAA_AVP_DIAMID_TYPE )
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      AAAAvpContainerEntry *e2 = em.acquire(AAA_AVP_STRING_TYPE);
      diameter_address_t& address = e->dataRef();
      std::string& str = e2->dataRef(Type2Type<diameter_octetstring_t>());

      ACE_UINT16 tmp = ntohs(address.type);
      char* c = (char*)&tmp;
      str.append(c, 2);
      str.append(address.value);

      setAppData(e2);

      try {
					OctetstringParser::parseAppToRaw();
      }
      catch (AAAErrorStatus st)
			{
	  			AAA_LOG(LM_ERROR, "error\n");
	  			setAppData(e);
	  			em.release(e2);
	  			throw;
			}
      setAppData(e);
      em.release(e2);
    }
};


class DiamidentParser : public Utf8stringParser
{
  public:

    void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAStringAvpContainerEntry *e;
      AAAErrorStatus st;
//      AAADictionaryEntry *avp = getDictData();
      getAppData(e);
      if (e->dataType() != AAA_AVP_DIAMID_TYPE )
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      e->dataType() = AAA_AVP_UTF8_STRING_TYPE;
      try {
	      Utf8stringParser::parseRawToApp();
      }
      catch (AAAErrorStatus st)	{
        e->dataType() = AAA_AVP_DIAMID_TYPE;
        throw st;
      }
      e->dataType() = AAA_AVP_DIAMID_TYPE;

//      std::string& str = e->dataRef();
    };

    void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAStringAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_UTF8_STRING_TYPE
      	  && e->dataType() != AAA_AVP_STRING_TYPE
      	  && e->dataType() != AAA_AVP_ADDRESS_TYPE
      	  && e->dataType() != AAA_AVP_DIAMID_TYPE )
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }

      AAAErrorStatus st;
//      AAADictionaryEntry *avp = getDictData();

//      std::string& str = e->dataRef();
      e->dataType() = AAA_AVP_UTF8_STRING_TYPE;
      try {
	      Utf8stringParser::parseAppToRaw();
      }
      catch (AAAErrorStatus st)	{
        e->dataType() = AAA_AVP_DIAMID_TYPE;
        throw st;
      }
    }
};

struct SchemeFunctor
{
  SchemeFunctor(diameter_uri_t &uri_) : uri(uri_) {}
  template <typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str(first,last);
    if (str == "aaa")
      uri.scheme=AAA_SCHEME_AAA;
    else
      uri.scheme=AAA_SCHEME_AAAS;
  }
  diameter_uri_t& uri;
};

struct TransportFunctor
{
  TransportFunctor(diameter_uri_t &uri_) : uri(uri_) {}
  template <typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str = std::string(first,last);
    if (str == "tcp")
      uri.transport=TRANSPORT_PROTO_TCP;
    else if (str == "udp")
      uri.transport=TRANSPORT_PROTO_UDP;
    else if (str == "sctp")
      uri.transport=TRANSPORT_PROTO_SCTP;
  }
  diameter_uri_t& uri;
};

struct ProtocolFunctor
{
  ProtocolFunctor(diameter_uri_t &uri_) : uri(uri_) {}
  template <typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str = std::string(first,last);
    if (str == "diameter")
      uri.transport=AAA_PROTO_DIAMETER;
    else if (str == "radius")
      uri.transport=AAA_PROTO_RADIUS;
    else if (str == "tacacs+")
      uri.transport=AAA_PROTO_TACACSPLUS;
  }
  diameter_uri_t& uri;
};

struct FqdnFunctor
{
  FqdnFunctor(diameter_uri_t &uri_) : uri(uri_) {}
  template <typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    uri.fqdn.assign(first, last);
  }
  diameter_uri_t& uri;
};


class DiamuriParser : public Utf8stringParser
{
  public:
    DiamuriParser()  {}
    void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAAvpContainerEntryManager em;
      AAADiamURIAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_DIAMURI_TYPE)
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      AAAAvpContainerEntry *e2 = em.acquire(AAA_AVP_UTF8_STRING_TYPE);
      setAppData(e2);    // Replace the original container entry with
			 // the new UTF8 string container entry.
//      AAADictionaryEntry *avp = getDictData();

      AAAErrorStatus st;
      try {
	      Utf8stringParser::parseRawToApp();
      }
      catch (AAAErrorStatus st)
      {
        setAppData(e); // Recover the original container entry.
        em.release(e2);// Release the UTF8 string container entry.
        throw st;
      }
      setAppData(e);     // Recover the original container entry.

//      std::string& str = e2->dataRef(Type2Type<diameter_utf8string_t>());
//      diameter_uri_t &uri = e->dataRef();
			 // grammer.uri data structure is copied into
			 // the container entry where the left
			 // variable refers to.
      em.release(e2);    // Release the UTF8 string container entry.
    };

    void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAAvpContainerEntryManager em;
      AAADiamURIAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_DIAMURI_TYPE)
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      AAAAvpContainerEntry *e2 = em.acquire(AAA_AVP_UTF8_STRING_TYPE);
      diameter_uri_t& uri = e->dataRef();
      std::string& str = e2->dataRef(Type2Type<diameter_octetstring_t>());

      char tmp[20];

      if (uri.scheme == AAA_SCHEME_AAA)
	    {
	     str.assign("aaa://");
	    }
      else
	    {
	      str.assign("aaas://");
	    }
      str.append(uri.fqdn);
      sprintf(tmp, ":%d", uri.port);
      str.append(tmp);
      sprintf(tmp, ";transport=%s",
	      uri.transport == TRANSPORT_PROTO_TCP ? "tcp" : "sctp");
      str.append(tmp);
      sprintf(tmp, ";protocol=%s",
	      uri.protocol == AAA_PROTO_DIAMETER ? "diameter" : "radius");
      str.append(tmp);

      setAppData(e2); // Replace the original container entry with the
		      // new UTF8 string container entry.
      try {
	      Utf8stringParser::parseAppToRaw();
      }
      catch (AAAErrorStatus st)
      {
        setAppData(e);  // Recover the original container entry.
        em.release(e2); // Release the UTF8 string container entry.
        throw st;
      }
      setAppData(e); // Recover the original container entry.
      em.release(e2);// Release the UTF8 string container entry.
    }
  private:

    diameter_uri_t uri_;
};

struct KeywordFunctor
{
  KeywordFunctor(AAA_UINT8 &ref) : ref(ref) {}
  template <typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str = std::string(first,last);
    if (str == "permit")
      ref = AAA_IPFILTER_RULE_ACTION_PERMIT;
    else if (str == "deny")
      ref = AAA_IPFILTER_RULE_ACTION_DENY;
    else if (str == "in")
      ref = AAA_IPFILTER_RULE_DIRECTION_IN;
    else if (str == "out")
      ref = AAA_IPFILTER_RULE_DIRECTION_OUT;
    else if (str == "any")
      ref = AAA_IPFILTER_RULE_SRCDST_KEYWORD_ANY;
    else if (str == "assigned")
      ref = AAA_IPFILTER_RULE_SRCDST_KEYWORD_ASSIGNED;
    AAA_LOG(LM_DEBUG, "IPFilterRule parser: keyword = %s\n", str.c_str());
  }
  template <typename IteratorT>
  void operator()(IteratorT val) const
  {
    if (val == '/')
      ref = AAA_IPFILTER_RULE_SRCDST_MASK;
    AAA_LOG(LM_DEBUG, "IPFilterRule parser: keyword = /\n");
  }
  AAA_UINT8 &ref;
};

struct IPAddressFunctor
{
  IPAddressFunctor(diameter_utf8string_t& ipno) : ipno(ipno) {}
  template <typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    ipno.assign(first,last);
  }
  diameter_utf8string_t& ipno;
};

struct Int8RangeListFunctor1
{
  Int8RangeListFunctor1(std::list<AAA_UINT8_RANGE> &l) : rangeList(l) {}
  template <typename T>
  void operator()(T const& val) const
  {
    rangeList.push_back(AAA_UINT8_RANGE(val, val));
  }
  std::list<AAA_UINT8_RANGE>& rangeList;
};

struct Int8RangeListFunctor2
{
  Int8RangeListFunctor2(std::list<AAA_UINT8_RANGE> &l) : rangeList(l) {}
  template <typename T>
  void operator()(T const& val) const
  {
    rangeList.back().last = val;
  }
  std::list<AAA_UINT8_RANGE>& rangeList;
};

struct Int16RangeListFunctor1
{
  Int16RangeListFunctor1(std::list<AAA_UINT16_RANGE> &l) : rangeList(l) {}
  template <typename T>
  void operator()(T const& val) const
  {
    rangeList.push_back(AAA_UINT16_RANGE(val, val));
  }
  std::list<AAA_UINT16_RANGE>& rangeList;
};

struct Int16RangeListFunctor2
{
  Int16RangeListFunctor2(std::list<AAA_UINT16_RANGE> &l) : rangeList(l) {}
  template <typename T>
  void operator()(T const& val) const
  {
    rangeList.back().last = val;
  }
  std::list<AAA_UINT16_RANGE>& rangeList;
};

struct BoolFunctor
{
  BoolFunctor(bool& ref, bool val) : ref(ref), value(val) {}
  template <typename T>
  void operator()(T const& first, T const& last) const
  {
    ref = value;
  }
  template <typename T>
  void operator()(T const& c) const
  {
    ref = value;
  }
  bool& ref;
  bool value;
};

struct IntListFunctor
{
  public:
  IntListFunctor(std::list<int>& l, int v) : iList(l), value(v) {}
  template <typename T>
  void operator()(T const& first, T const& last) const
  {
    iList.push_back(value);
  }
  std::list<int>& iList;
  int value;
};


class IPFilterRuleParser : public Utf8stringParser
{
  public:
    IPFilterRuleParser()  {}
    void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAAvpContainerEntryManager em;
      AAAIPFilterRuleAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_IPFILTER_RULE_TYPE)
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      AAAAvpContainerEntry *e2 = em.acquire(AAA_AVP_UTF8_STRING_TYPE);
      setAppData(e2);    // Replace the original container entry with
			 // the new UTF8 string container entry.
//      AAADictionaryEntry *avp = getDictData();

      AAAErrorStatus st;
      try {
	      Utf8stringParser::parseRawToApp();
      }
      catch (AAAErrorStatus st)
      {
        setAppData(e); // Recover the original container entry.
        em.release(e2);// Release the UTF8 string container entry.
        throw st;
      }
      setAppData(e);     // Recover the original container entry.

//      std::string& str = e2->dataRef(Type2Type<diameter_utf8string_t>());
//      diameter_ipfilter_rule_t &ipfilter_rule = e->dataRef();
      // Set default values.
      em.release(e2);    // Release the UTF8 string container entry.
    };

    void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAAvpContainerEntryManager em;
      AAAIPFilterRuleAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_IPFILTER_RULE_TYPE)
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      AAAAvpContainerEntry *e2 = em.acquire(AAA_AVP_UTF8_STRING_TYPE);
      diameter_ipfilter_rule_t& r = e->dataRef();
      std::string& str = e2->dataRef(Type2Type<diameter_utf8string_t>());

      if (r.action == AAA_IPFILTER_RULE_ACTION_PERMIT)
	      str.append("permit ");
      else
	      str.append("deny ");
      if (r.dir == AAA_IPFILTER_RULE_DIRECTION_IN)
	      str.append("in ");
      else
	      str.append("out ");
      if (r.proto == 0)
	      str.append("ip ");
      else
      {
        char protoStr[4];
        sprintf(protoStr, "%u ", r.proto);
        str.append(protoStr);
      }
      str.append("from ");
      if (!r.src.modifier)
	      str.append("!");
      if (r.src.representation == AAA_IPFILTER_RULE_SRCDST_KEYWORD_ANY)
	      str.append("any ");
      else if (r.src.representation
	      == AAA_IPFILTER_RULE_SRCDST_KEYWORD_ASSIGNED)
 	      str.append("assigned ");
      else {
      	str.append(r.src.ipno);
        if (r.src.representation == AAA_IPFILTER_RULE_SRCDST_MASK)
        {
          char bitsStr[5];
          sprintf(bitsStr, "/%u", r.src.bits);
          str.append(bitsStr);
        }
	      str.append(" ");
      }
      for (std::list<AAA_UINT16_RANGE>::iterator i=r.src.portRangeList.begin();
	      i!=r.src.portRangeList.end();)
      {
        char portStr[12];
        if ((*i).first == (*i).last)
        {
          sprintf(portStr, "%u", (*i).first);
          str.append(portStr);
        }
        else
        {
          sprintf(portStr, "%u-%u", (*i).first, (*i).last);
          str.append(portStr);
        }
        if (++i != r.src.portRangeList.end())
          str.append(",");
        else
          str.append(" ");
      }

      str.append("to ");
      if (!r.dst.modifier)
       	str.append("!");
      if (r.dst.representation == AAA_IPFILTER_RULE_SRCDST_KEYWORD_ANY)
	      str.append("any ");
      else if (r.dst.representation
	      == AAA_IPFILTER_RULE_SRCDST_KEYWORD_ASSIGNED)
      	str.append("assigned ");
      else {
	      str.append(r.dst.ipno);
        if (r.dst.representation == AAA_IPFILTER_RULE_SRCDST_MASK)
        {
          char bitsStr[5];
          sprintf(bitsStr, "/%u", r.dst.bits);
          str.append(bitsStr);
        }
	      str.append(" ");
      }
      for (std::list<AAA_UINT16_RANGE>::iterator i=r.dst.portRangeList.begin();
	      i!=r.dst.portRangeList.end();)
      {
        char portStr[12];
        if ((*i).first == (*i).last)
          sprintf(portStr, "%u", (*i).first);
        else
          sprintf(portStr, "%u-%u", (*i).first, (*i).last);
        str.append(portStr);
        if (++i != r.dst.portRangeList.end())
          str.append(",");
        else
          str.append(" ");
      }
      if (r.frag)
	      str.append(" frag");

      if (r.ipOptionList.size()>0)
	      str.append(" ipoptions");

      for (std::list<int>::iterator i=r.ipOptionList.begin();
	      i!=r.ipOptionList.end();)
      {
        bool negation=false;
        int opt = *i;
        if (opt < 0)
        {
          negation=true;
          opt = -opt;
        }
        if (negation)
          str.append("!");
        if (opt == AAA_IPFILTER_RULE_IP_OPTION_SSRR)
          str.append("ssrr");
        else if (opt == AAA_IPFILTER_RULE_IP_OPTION_LSRR)
          str.append("lsrr");
        else if (opt == AAA_IPFILTER_RULE_IP_OPTION_RR)
          str.append("rr");
        else if (opt == AAA_IPFILTER_RULE_IP_OPTION_TS)
          str.append("ts");
        if (++i != r.ipOptionList.end())
          str.append(",");
      }


      if (r.tcpOptionList.size()>0)
	      str.append(" tcpoptions ");

      for (std::list<int>::iterator i=r.tcpOptionList.begin();
	      i!=r.tcpOptionList.end();)
      {
        bool negation=false;
        int opt = *i;
        if (opt < 0)
        {
          negation=true;
          opt = -opt;
        }
        if (negation)
          str.append("!");
        if (opt == AAA_IPFILTER_RULE_TCP_OPTION_MSS)
          str.append("mss");
        else if (opt == AAA_IPFILTER_RULE_TCP_OPTION_WINDOW)
          str.append("window");
        else if (opt == AAA_IPFILTER_RULE_TCP_OPTION_SACK)
          str.append("sack");
        else if (opt == AAA_IPFILTER_RULE_TCP_OPTION_TS)
          str.append("ts");
        else if (opt == AAA_IPFILTER_RULE_TCP_OPTION_CC)
          str.append("cc");
        if (++i != r.tcpOptionList.end())
          str.append(",");
      }


      if (r.established)
	      str.append(" established");

      if (r.setup)
	      str.append(" setup");

      if (r.tcpFlagList.size()>0)
	      str.append(" tcpflags");

      for (std::list<int>::iterator i=r.tcpFlagList.begin();
	      i!=r.tcpFlagList.end();)
      {
        bool negation=false;
        int opt = *i;
        if (opt < 0)
          {
            negation=true;
            opt = -opt;
          }
        if (negation)
          str.append("!");
        if (opt == AAA_IPFILTER_RULE_TCP_FLAG_FIN)
          str.append("fin");
        else if (opt == AAA_IPFILTER_RULE_TCP_FLAG_SYN)
          str.append("syn");
        else if (opt == AAA_IPFILTER_RULE_TCP_FLAG_RST)
          str.append("rst");
        else if (opt == AAA_IPFILTER_RULE_TCP_FLAG_PSH)
          str.append("psh");
        else if (opt == AAA_IPFILTER_RULE_TCP_FLAG_ACK)
          str.append("ack");
        else if (opt == AAA_IPFILTER_RULE_TCP_FLAG_URG)
          str.append("urg");
        if (++i != r.tcpFlagList.end())
          str.append(",");
      }

      if (r.icmpTypeRangeList.size()>0)
       	str.append(" icmptypes");

      for (std::list<AAA_UINT8_RANGE>::iterator i=r.icmpTypeRangeList.begin();
	      i!=r.icmpTypeRangeList.end();)
      {
        char typeStr[8];
        if ((*i).first == (*i).last)
          sprintf(typeStr, "%u", (*i).first);
        else
          sprintf(typeStr, "%u-%u", (*i).first, (*i).last);
        str.append(typeStr);
        if (++i != r.icmpTypeRangeList.end())
          str.append(",");
      }

      AAA_LOG(LM_DEBUG, "Set IPFilterRule = \"%s\"\n", str.c_str());


      setAppData(e2); // Replace the original container entry with the
		      // new UTF8 string container entry.
      try {
	      Utf8stringParser::parseAppToRaw();
      }
      catch (AAAErrorStatus st)
      {
        setAppData(e);  // Recover the original container entry.
        em.release(e2); // Release the UTF8 string container entry.
        throw st;
      }
      setAppData(e); // Recover the original container entry.
      em.release(e2);// Release the UTF8 string container entry.
    }
  private:
    diameter_ipfilter_rule_t ipfilter_rule_;
};

class GroupedParser : public AvpValueParser
{
  public:
    void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAMessageBlock* aBuffer = getRawData();
      AAAGroupedAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_GROUPED_TYPE)
	    {
	      AAAErrorStatus st;
	      AAA_LOG(LM_ERROR, "Invalid AVP type.");
	      st.set(BUG, PROHIBITED_CONTAINER);
	      throw st;
	    }
      AAADictionaryEntry *avp = getDictData();

      AAAAvpContainerList* acl = e->dataPtr();
      AAAGroupedAVP* gavp;
      AAAErrorStatus st;

      /* find grouped avp structure */
#ifdef DEBUG
      printf("Getting Grouped AVP:[%s]\n", avp->avpName.c_str());
#endif

      if ((gavp = AAAGroupedAvpList::instance()
	      ->search(avp->avpCode, avp->vendorId, avp->scenarioId)) == NULL)
	    {
	      AAA_LOG(LM_ERROR, "Grouped AVP [%d], scenario:[%d] not found.", avp->avpCode, avp->scenarioId);
	      st.set(NORMAL, AAA_AVP_UNSUPPORTED, avp);
	      throw st;
	    }

      do {
	      QualifiedAvpListParser qc;
	      qc.setRawData(aBuffer);
	      qc.setAppData(acl);
	      qc.setDictData(gavp);

	      try {
	        qc.parseRawToApp();
	      }
	      catch (AAAErrorStatus st) {
	        AAA_LOG(LM_ERROR, "Grouped AVP failure");
	        throw;
	      }
      } while (0);
    };

    void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAMessageBlock* aBuffer = getRawData();
      AAAGroupedAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_GROUPED_TYPE)
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }
      AAADictionaryEntry *avp = getDictData();

      AAAAvpContainerList *acl = e->dataPtr();
      AAAGroupedAVP* gavp;
      AAAErrorStatus st;

     if ((gavp = AAAGroupedAvpList::instance()
	   ->search(avp->avpCode, avp->vendorId, avp->scenarioId)) == NULL)
	   {
	     AAA_LOG(LM_ERROR, "Grouped AVP [%d], scenarioId:[%d] not found", avp->avpCode, avp->scenarioId);
	     st.set(NORMAL, AAA_AVP_UNSUPPORTED, avp);
	     throw st;
	   }

     do {
       QualifiedAvpListParser qc;
       qc.setRawData(aBuffer);
       qc.setAppData(acl);
       qc.setDictData(gavp);

       try {
         qc.parseAppToRaw();
       }
       catch (AAAErrorStatus st) {
         AAA_LOG(LM_ERROR, "Grouped AVP failure");
         throw;
       }
     } while (0);
   }
};

/* In RFC3588:

      Time
         The Time format is derived from the OctetString AVP Base
         Format. The string MUST contain four octets, in the same format
         as the first four bytes are in the NTP timestamp format. The
         NTP Timestamp format is defined in chapter 3 of [SNTP].

         This represents the number of seconds since 0h on 1 January
         1900 with respect to the Coordinated Universal Time (UTC).

         On 6h 28m 16s UTC, 7 February 2036 the time value will
         oflow. SNTP [SNTP] describes a procedure to extend the time
         to 2104. This procedure MUST be supported by all DIAMETER
         nodes.
 */
/* And from RFC2030 (SNTP):

3. NTP Timestamp Format

   SNTP uses the standard NTP timestamp format described in RFC-1305 and
   previous versions of that document. In conformance with standard
   Internet practice, NTP data are specified as integer or fixed-point
   quantities, with bits numbered in big-endian fashion from 0 starting
   at the left, or high-order, position. Unless specified otherwise, all
   quantities are unsigned and may occupy the full field width with an
   implied 0 preceding bit 0.

   Since NTP timestamps are cherished data and, in fact, represent the
   main product of the protocol, a special timestamp format has been
   established. NTP timestamps are represented as a 64-bit unsigned
   fixed-point number, in seconds relative to 0h on 1 January 1900. The
   integer part is in the first 32 bits and the fraction part in the
   last 32 bits. In the fraction part, the non-significant low order can
   be set to 0.

      It is advisable to fill the non-significant low order bits of the
      timestamp with a random, unbiased bitstring, both to avoid
      systematic roundoff errors and as a means of loop detection and
      replay detection (see below). One way of doing this is to generate
      a random bitstring in a 64-bit word, then perform an arithmetic
      right shift a number of bits equal to the number of significant
      bits of the timestamp, then add the result to the original
      timestamp.

   This format allows convenient multiple-precision arithmetic and
   conversion to UDP/TIME representation (seconds), but does complicate
   the conversion to ICMP Timestamp message representation, which is in
   milliseconds. The maximum number that can be represented is
   4,294,967,295 seconds with a precision of about 200 picoseconds,
   which should be adequate for even the most exotic requirements.

                        1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                           Seconds                             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                  Seconds Fraction (0-padded)                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

   Note that, since some time in 1968 (second 2,147,483,648) the most
   significant bit (bit 0 of the integer part) has been set and that the
   64-bit field will overflow some time in 2036 (second 4,294,967,296).
   Should NTP or SNTP be in use in 2036, some external means will be
   necessary to qualify time relative to 1900 and time relative to 2036
   (and other multiples of 136 years). There will exist a 200-picosecond
   interval, henceforth ignored, every 136 years when the 64-bit field
   will be 0, which by convention is interpreted as an invalid or
   unavailable timestamp.

      As the NTP timestamp format has been in use for the last 17 years,
      it remains a possibility that it will be in use 40 years from now
      when the seconds field overflows. As it is probably inappropriate
      to archive NTP timestamps before bit 0 was set in 1968, a
      convenient way to extend the useful life of NTP timestamps is the
      following convention: If bit 0 is set, the UTC time is in the
      range 1968-2036 and UTC time is reckoned from 0h 0m 0s UTC on 1
      January 1900. If bit 0 is not set, the time is in the range 2036-
      2104 and UTC time is reckoned from 6h 28m 16s UTC on 7 February
      2036. Note that when calculating the correspondence, 2000 is not a
      leap year. Note also that leap seconds are not counted in the
      reckoning.
 */
/// The timer parser implementation.  Since the time structure can be
/// simply parsed as a simple 32-bit unsinged integer, the parser
/// treats the data as Unsigned32 type instead of OctetString.
#if 0
class TimeParser : public OctetstringParser
{
  public:

    void parseRawToApp() throw(AAAErrorStatus)
    {
      AAAAvpContainerEntryManager em;
      AAAErrorStatus st;
      AAATimeAvpContainerEntry *e; // Time data.
      getAppData(e);
      if (e->dataType() != AAA_AVP_TIME_TYPE)
	    {
	      AAAErrorStatus st;
	      AAA_LOG(LM_ERROR, "Invalid AVP type.");
	      st.set(BUG, PROHIBITED_CONTAINER);
	      throw st;
	    }

      AAAAvpContainerEntry *e2 = em.acquire(AAA_AVP_STRING_TYPE);
      setAppData(e2);  // Replace the original time container entry
		       // with a string container entry.

      try {
	      OctetstringParser::parseRawToApp();
      }
      catch (AAAErrorStatus st)
	    {
	      setAppData(e);  // Restore the original container entry.
	      em.release(e2); // Release the string container entry.
	      throw st;
	    }

      std::string& str = e2->dataRef(Type2Type<diameter_octetstring_t>());
      diameter_time_t& time = e->dataRef();
      time = ntohl32(*(diameter_time_t*)str.data());

      setAppData(e);  // Restore the original container entry.
      em.release(e2); // Release the string container entry.
    };

    void parseAppToRaw() throw(AAAErrorStatus)
    {
      AAAAvpContainerEntryManager em;
      AAAErrorStatus st;
      AAATimeAvpContainerEntry *e;
      getAppData(e);
      if (e->dataType() != AAA_AVP_TIME_TYPE)
      {
        AAAErrorStatus st;
        AAA_LOG(LM_ERROR, "Invalid AVP type.");
        st.set(BUG, PROHIBITED_CONTAINER);
        throw st;
      }

      AAAAvpContainerEntry *e2 = em.acquire(AAA_AVP_STRING_TYPE);
      setAppData(e2);  // Replace the original time container entry
		       // with a string container entry.

      std::string& str = e2->dataRef(Type2Type<diameter_octetstring_t>());
      diameter_time_t& time = e->dataRef();
      UINT32 timeNBO; // time represented by Network Byte Order.
      timeNBO = ntohl32(time);

      str = std::string((char*)timeNBO, sizeof(timeNBO));

      try {
	      OctetstringParser::parseAppToRaw();
      }
      catch (AAAErrorStatus st)
      {
        setAppData(e);  // Restore the original container entry.
        em.release(e2); // Release the string container entry.
        throw st;
      }
      setAppData(e);  // Restore the original container entry.
      em.release(e2); // Release the string container entry.
    }
};
#endif
#endif // __PARSER_AVPVALUE_H__

