#ifndef __PARSER_Q_AVPLIST_H__
#define __PARSER_Q_AVPLIST_H__

#include "diameter_parser_api.h"

typedef AAAParser<AAAMessageBlock*, AAAAvpContainerList*, AAADictionary*>
QualifiedAvpListParser;
#ifdef __BORLANDC__
template<>
void
QualifiedAvpListParser::parseRawToApp();
template<>
void
QualifiedAvpListParser::parseAppToRaw();
#endif
#endif
