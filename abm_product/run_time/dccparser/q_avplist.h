#ifndef __Q_AVPLIST_H__
#define __Q_AVPLIST_H__

#include <list>
#include "diameter_parser_api.h"
#include "avplist.h"

struct qual
{
  ACE_UINT16 min;
  ACE_UINT16 max;
};

#define QUAL_INFINITY 65535 /* 2^16 -1 */

typedef struct /* AVP with qualifier (min,max) */
{
  AAADictionaryEntry *avp;
  struct qual qual;
} AAAQualifiedAVP;

class AAAQualifiedAvpList: public std::list<AAAQualifiedAVP*>
{
  public:
    AAAQualifiedAvpList(AAAAvpParseType pt) { parseType = pt; };
    ~AAAQualifiedAvpList();
    inline void add(AAAQualifiedAVP* q_avp) { push_back(q_avp); }
    unsigned getMinSize(void);
    inline AAAAvpParseType& getParseType(void) { return parseType; };
  private:
    AAAAvpParseType parseType;
};

struct four_qavp_l {
  AAAQualifiedAvpList *avp_f;  
  AAAQualifiedAvpList *avp_r;  
  AAAQualifiedAvpList *avp_o;  
  AAAQualifiedAvpList *avp_f2;  
};


#endif // __Q_AVPLIST_H__
