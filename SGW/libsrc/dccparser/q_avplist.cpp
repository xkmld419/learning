#include <stdlib.h>
#include "q_avplist.h"
#include "parser.h"
#include "parser_avp.h"

AAAQualifiedAvpList::~AAAQualifiedAvpList()
{
  for (iterator i=begin(); i!=end(); i++) { delete *i; }
}

unsigned 
AAAQualifiedAvpList::getMinSize(void)
{
  unsigned sum=0;
  for (iterator c=begin(); c!=end(); c++)
  {
    sum += (::getMinSize((*c)->avp)) * ((*c)->qual.min);
  }
  return sum;
}

