#ifndef __COMLIST_H__
#define __COMLIST_H__

#include "diameter_parser_api.h"
#include "q_avplist.h"

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

struct comflags
{
  AAA_UINT8 r:1;
  AAA_UINT8 p:1;
  AAA_UINT8 e:1;
  
};

class AAACommand : public AAADictionary
{
 public:
  std::string name;
  struct comflags flags;
  std::string scenario;
  int protocol;  
};

class AAACommandList : public std::list<AAACommand*>
{
  public:
 	  static AAACommandList * instance() 
	  { 
      if( instance_ == 0 ) 
        instance_ = new AAACommandList;
      return instance_ ; 
    }
 	
    void add(AAACommand*);
    AAACommand* search(const char*name, int protocol = -1, const char *scenario = "");
    AAACommand* search(ACE_UINT32 code, ACE_UINT32 appId,
                     int request, int protocol = -1, const char *scenario = "");
    void release();

  private:
    AAACommandList() {}
    ~AAACommandList();
    static AAACommandList *instance_;

};

/* class AAACommandList_S : public std::list<AAACommand*>
{
//  friend class ACE_Singleton<AAACommandList_S, ACE_Recursive_Thread_Mutex>;
  friend class ACE_Singleton<AAACommandList_S, ACE_Null_Mutex>;
 public:
  void add(AAACommand*);
  AAACommand* search(const char*name, int protocol = -1, const char *scenario = "");
  AAACommand* search(ACE_UINT32 code, ACE_UINT32 appId,
                     int request, int protocol = -1, const char *scenario = "");
  void release();

 private:
  AAACommandList_S() {}
  ~AAACommandList_S();
  ACE_Null_Mutex mutex;
};
//typedef ACE_Singleton<AAACommandList_S, ACE_Recursive_Thread_Mutex> AAACommandList;
typedef ACE_Singleton<AAACommandList_S, ACE_Null_Mutex> AAACommandList;
*/

#endif // __COMLIST_H__
