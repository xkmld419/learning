#ifndef __PARSER_AVP_H__
#define __PARSER_AVP_H__

#include "avplist.h"
#include "parser.h"
#include "avp_header.h"
#include "diameter_parser_api.h"
#include "q_avplist.h"
#include "comlist.h"

class AvpRawData 
{
  public:
    union 
    {
      AAAMessageBlock *msg;
      AAAAvpHeaderList *ahl;
    };
};

typedef AAAParser<AvpRawData*, AAAAvpHeader*, AAADictionaryEntry*> AvpHeaderParser;
typedef AAAParser<AvpRawData*, AAAAvpContainer*, AAADictionaryEntry*> AvpParser;
#ifdef __BORLANDC__
template<>
void
AvpHeaderParser::parseRawToApp();
template<>
void
AvpHeaderParser::parseAppToRaw();
template<>
void
AvpParser::parseRawToApp();
template<>
void
AvpParser::parseAppToRaw();
#endif
#endif // __PARSER_AVP_H__
