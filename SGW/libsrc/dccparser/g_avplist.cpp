#include <stdio.h>
#include <stdlib.h>
#include "avplist.h"
#include "g_avplist.h"

AAAGroupedAvpList::~AAAGroupedAvpList()
{
  for (iterator i=begin(); i!=end(); i++)
  {
    delete (*i)->avp_f;
    delete (*i)->avp_r;
    delete (*i)->avp_o;
    delete (*i)->avp_f2;
    delete *i;
  }
}

void AAAGroupedAvpList::release()
{
  for (iterator i=begin(); i!=end(); i++)
  {
    delete (*i)->avp_f;
    delete (*i)->avp_r;
    delete (*i)->avp_o;
    delete (*i)->avp_f2;
    delete *i;
  }
  erase(begin(), end());
}

void
AAAGroupedAvpList::add(AAAGroupedAVP *gavp)
{
  if (search(gavp->code, gavp->vendorId, gavp->scenarioId) != NULL)
  {
    AAA_LOG(LM_ERROR, "duplicated grouped AVP,code=%d,scenario=%d.\n", gavp->code, gavp->scenarioId);
    exit(1);
  }
//  mutex.acquire();
  push_back(gavp);
//  mutex.release();
}

AAAGroupedAVP*
AAAGroupedAvpList::search(ACE_UINT32 code, ACE_UINT32 vendorId, ACE_UINT32 scenario_id)
{
//  mutex.acquire();
  for (iterator c=begin(); c!=end(); c++)
  {
    if ((*c)->code == code && (*c)->vendorId == vendorId && (*c)->scenarioId == scenario_id)
	  {
//	    mutex.release();
	    return *c;
	  }
  }
//  mutex.release();
  return NULL;
}


