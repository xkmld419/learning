#include "avplist.h"
#include "comlist.h"
#include "xmldict.h"
#include "parser_avpvalue.h"

AAACommandList::~AAACommandList()
{
	printf("~AAACommandList...\n");
  for (iterator i=begin(); i!=end(); i++)
  {
      //      delete (*i)->name;
    delete (*i)->avp_f;
    delete (*i)->avp_r;
    delete (*i)->avp_o;
    delete (*i)->avp_f2;
    delete *i;
  }
}

void AAACommandList::release()
{
	
	printf("AAACommandList release...\n");
  for (iterator i=begin(); i!=end(); i++)
  {
      //      delete (*i)->name;
    delete (*i)->avp_f;
    delete (*i)->avp_r;
    delete (*i)->avp_o;
    delete (*i)->avp_f2;
    delete *i;
  }
  erase(begin(), end());

}

void
AAACommandList::add(AAACommand *com)
{
  if (search(com->name.c_str(), com->protocol, com->scenario.c_str()) != NULL)
  {
    AAA_LOG(LM_ERROR, "duplicated command:[%s],scenario:[%s] definition.\n", com->name.c_str(), com->scenario.c_str());
    exit(1);
  }
  
 // mutex.acquire();
  push_back(com);
//  mutex.release();
#ifdef DEBUG
  std::cout << "add Command name = " << com->name << ", size:" << size() << "\n"; 
#endif
}

AAACommand*
AAACommandList::search(const char *name, int protocol, const char *scenario )  // search by name 
{
//  mutex.acquire();
//  AAA_LOG(LM_ERROR, "scenario=%s.\n", scenario);
  for (iterator c=begin(); c!=end(); c++)
  {
    if ((*c)->name == std::string(name) && (*c)->scenario == std::string(scenario))
	  {
      if ((protocol >= 0) && ((*c)->protocol != protocol)) {
        continue;
      }
//	    mutex.release();
	    return *c;
	  }
  }
//  mutex.release();
  return NULL;
}


AAACommand*
AAACommandList::search(ACE_UINT32 code, ACE_UINT32 appId,
                         int request, int protocol, const char *scenario )  
  // search by code and applicationId
{
//  mutex.acquire();

  for (iterator c=begin(); c!=end(); c++)
  {
    if ((*c)->code == code && 
	    (*c)->appId == appId &&
	    (*c)->flags.r == request && 
	    (*c)->scenario == std::string(scenario) )
	  {
      if ((protocol >= 0) && ((*c)->protocol != protocol)) {
        continue;
      }
//	    mutex.release();
	    return *c;
	  }
  }
//  mutex.release();
  return NULL;
}

boolean_t 
AAADictionaryManager::getCommandCode(char *commandName,
				     AAACommandCode *commandCode,
				     AAAApplicationId *appId)
{
  AAACommand *com;
  if ((com = AAACommandList::instance()->search
       (commandName, protocol)) == NULL)
    {
      return false;
    }
  *commandCode = com->code;
  *appId = com->appId;
  return true;
}

boolean_t getCommandCode(const char *commandName,
	AAACommandCode& commandCode,
	AAAApplicationId& appId)
{
  AAACommand *com;
  if ((com = AAACommandList::instance()->search
       (commandName)) == NULL)
  {
    return false;
  }
  commandCode = com->code;
  appId = com->appId;
  return true;
}


void AAADictionaryManager::init(char *dictFile)
{
  // Parser the XML dictionary.
//  printf("1.comlist size=%d\n", AAACommandList::instance()->size());
//  printf("1.avplist size=%d\n", AAAAvpList::instance()->size());
  parseXMLDictionary(dictFile);
//  printf("3.comlist size=%d\n", AAACommandList::instance()->size());
//  printf("3.avplist size=%d\n", AAAAvpList::instance()->size());
	
}

void RemoveDictionary()
{
	AAACommandList::instance()->release();
	AAAAvpList::instance()->release();
	AvpEnumeratedInfo::instance()->release();
	AAAGroupedAvpList::instance()->release();
}

/* void RemoveDictionary()
{
}*/

AAADictionaryHandle *AAADictionaryManager::getDictHandle
(AAACommandCode code, AAAApplicationId id, int rflag)
{
  return AAACommandList::instance()->search(code, id, rflag, protocol);
}


AAADictionaryHandle *AAADictionaryManager::getDictHandle(char *cmdName)
{
  return AAACommandList::instance()->search(cmdName, protocol);
}

