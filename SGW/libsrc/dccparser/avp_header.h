#ifndef __AVP_HEADER_H__
#define __AVP_HEADER_H__

#include <list>
#include <string>
#include "parser.h"
#include "diameter_parser_api.h"

struct avp_flag {
  AAA_UINT8 v;
  AAA_UINT8 m;
  AAA_UINT8 p;
};

class AAAAvpHeader
{
 public:
  ACE_UINT32 code;
  struct avp_flag flag;
  ACE_UINT32 length:24;
  ACE_UINT32 vendor;
  char* value_p;
  inline AAAAvpParseType& ParseType() { return parseType; }
 private:
  AAAAvpParseType parseType;
}; 

#define adjust_word_boundary(len)  ((ACE_UINT32)((((len-1)>>2)+1)<<2))

class AAAAvpHeaderList : public std::list<AAAAvpHeader>
{
 public:
  void create(AAAMessageBlock *aBuffer) throw(AAAErrorStatus);
};

#endif // __AVP_HEADER_H__


