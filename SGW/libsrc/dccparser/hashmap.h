#ifndef __COMLIST_H__
#define __COMLIST_H__

#include <hash_map>
#include "diameter_parser_api.h"

class AAADictionary : public AAADictionaryHandle
{
 public:
  AAACommandCode code;
  AAAApplicationId appId;
  AAAVendorId vendorId;
  AAAVendorId scenarioId;
  AAAQualifiedAvpList* avp_f;  /* fixed */
  AAAQualifiedAvpList* avp_r;  /* required */
  AAAQualifiedAvpList* avp_o;  /* optional */
  AAAQualifiedAvpList* avp_f2; /* fixed */
};

typedef struct AAAAvpCode
{
  AAA_AVPCode     avpCode;  /**< AVP code */
  std::string     avpName;
  AAA_AVPDataType avpType;  /**< AVP type */
  AAAVendorId     vendorId; /**< Vendor ID */
  int             scenarioId; /**< Protocol */  
  AAA_AVPFlag     flags;    /**< AVP flags */
  int             protocol; /**< Protocol */  
};

class AvpCodeHashMap_S
{
	
	
  friend class ACE_Singleton<AvpCodeHashMap_S, ACE_Null_Mutex> ;

 public:
  struct eqAvpCode
  {
	  bool operator() (const AAA_AVPCode s1, const AAA_AVPCode s2) const
	  {
		  return (s1==s2);
	  }
  };

  struct eqAvpName
  {
	  bool operator() (const std::string &s1, const std::string &s2) const
	  {
		  return ( s1 == s2 );
	  }
  };

	std::hash_map<const AAA_AVPCode, AAAAvpCode *, hash<AAAAvpCode *>, eqAvpCode> avpCodeHashmap;
	std::hash_map<const std::string, AAAAvpCode *, hash<AAAAvpCode *>, eqAvpName> avpNameHashmap;
  add();
  release();
  AvpCodeHashMap_S() {}
  ~AvpCodeHashMap_S();

 private:
	 static std::vector<AAAAvpCode> v_item;
   ACE_Thread_Mutex mutex;
};

typedef ACE_Singleton<AvpCodeHashMap_S, ACE_Null_Mutex> AvpCodeHashMap;

#endif // __COMLIST_H__
