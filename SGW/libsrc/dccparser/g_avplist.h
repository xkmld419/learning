#ifndef __G_AVPLIST_H__
#define __G_AVPLIST_H__

#include <list>
#include "q_avplist.h"
#include "comlist.h"

class AAAGroupedAVP : public AAADictionary {};

class AAAGroupedAvpList : public std::list<AAAGroupedAVP*>
{
//  friend class ACE_Singleton<AAAGroupedAvpList, ACE_Recursive_Thread_Mutex>;
//  friend class ACE_Singleton<AAAGroupedAvpList, ACE_Null_Mutex>;
  public:
 	  static AAAGroupedAvpList * instance() 
	  { 
      if( instance_ == 0 ) 
        instance_ = new AAAGroupedAvpList;
      return instance_ ; 
    }
  	
    void add(AAAGroupedAVP*);
    AAAGroupedAVP* search(ACE_UINT32, ACE_UINT32, ACE_UINT32 id = 0);
    void release();
  private:
    AAAGroupedAvpList() {}
    ~AAAGroupedAvpList();
    static AAAGroupedAvpList *instance_;
//    ACE_Null_Mutex mutex;
};

//typedef ACE_Singleton<AAAGroupedAvpList, ACE_Recursive_Thread_Mutex> 
//typedef ACE_Singleton<AAAGroupedAvpList, ACE_Null_Mutex> 
//AAAGroupedAvpList;

#endif // __G_AVPLIST_H__


