#include "diameter_parser_api.h"

void
AAAAvpContainer::remove(AAAAvpContainerEntry* e)
{
  for (iterator i=begin(); i!=end(); i++)
  {
    if (*i == e)
    {
      erase(i);
      return;
    }
  }
}

AAAAvpContainerList::~AAAAvpContainerList()
{
  for (iterator i=begin(); i!=end(); i++)
  {
    AAAAvpContainer *c = *i;
    c->releaseEntries();
    delete c;
  }
  erase(begin(), end());
}

AAAAvpContainer*
AAAAvpContainerList::search(const char *name)
{
  for (iterator i=begin(); i!=end();i++)
  {
    AAAAvpContainer *c = *i;

    if (strcmp(c->getAvpName(), name) == 0)
	  return c;
  }
  return NULL;
}

AAAAvpContainer*
AAAAvpContainerList::search(bool b)
{
  for (iterator i=begin(); i!=end(); i++)
  {
    AAAAvpContainer *c = *i;
    if (c->flag == b) { return c; }
  }
  return NULL;
}

AAAAvpContainer*
AAAAvpContainerList::search(const char *name, bool b)
{
  for (iterator i=begin(); i!=end(); i++)
  {
    AAAAvpContainer *c = *i;
    if (c->flag == b && strcmp(c->getAvpName(), name) == 0)
		  return c;
  }
  return NULL;
}

AAAAvpContainer *
AAAAvpContainerList::search(AAADictionaryEntry *avp)
{
  AAAAvpContainer *c;
  if ((c = search(avp->avpName.c_str(), false)) == NULL)
  {
      return NULL;
  }
//#####################  c->flag = ! c->flag;
  return c;
}

void
AAAAvpContainerList::reset()
{
  for (iterator i=begin(); i!=end(); i++)
  {
    AAAAvpContainer *c = *i;
    c->flag = false;
  }
}

void
AAAAvpContainerList::releaseContainers()
{
  for (iterator i=begin(); i!=end(); i++)
  {
    AAAAvpContainer *c = *i;
    c->releaseEntries();
    delete c;
  }
  erase(begin(), end());
}

AAAAvpContainerEntry*
AAAAvpContainerEntryManager::acquire(AAA_AVPDataType type)
{
  AAAAvpContainerEntry *e;
  // Search creator;
  AvpType *avpType =  AvpTypeList::instance()->search(type);
  if (avpType == NULL)
  {
    AAAErrorStatus st;
    AAA_LOG(LM_ERROR, "Pre-defined type not found", type);
    st.set(BUG, INVALID_CONTAINER_PARAM);
    throw st;
  }
  e = avpType->createContainerEntry(type);
  return e;
}

AAAAvpContainer*
AAAAvpContainerManager::acquire(const char *avpName)
{
  AAAAvpContainer *c = new AAAAvpContainer;
  c->setAvpName(avpName);
  return (c);
}

void
AAAAvpContainerManager::release(AAAAvpContainer *c)
{
  delete c;
}
