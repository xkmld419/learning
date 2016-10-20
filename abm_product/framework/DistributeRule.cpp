/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "DistributeRule.h"

//##ModelId=424CFCE6018D
DistributeRule::DistributeRule(MessageQueue *poMQ, int conID, DistributeRule *next, int iOutProcessID)
:m_iConditionID (conID), m_poMQ (poMQ), m_poNext (next), m_iOutProcessID(iOutProcessID), m_poSendRecord (0)
{
}

