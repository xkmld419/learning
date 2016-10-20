
#ifndef _P_MESSAGESEND_H_
#define _P_MESSAGESEND_H_


#include <iostream>
#include <unistd.h>
#include <stdio.h>
//#include "petri.h"
#include "string.h"
namespace PETRI {

class message
{
	public:
		
	void loadInMessage();
	//int AutoPlaceSendMessage(long transID,string inState);
	int noFireSendMessage();
	int TimeOutWriteToBSN();

	public:

	long m_RULE_ID[1024];
	long m_TRANSITION_ID[1024];
	long m_STAFF_GROUP_ID[1024];
	long m_BEGIN_DEFER[1024];
	long m_TIME_OFFSET[1024];
	long m_COUNT[1024];
	long m_INTERVAL[1024];	
	long m_TYPE_TIME[1024] ;
	long m_TYPE_BEGIN[1024];	
	long m_TYPE_END[1024];	
	long m_TYPE_ERR[1024];
	int m_size;
			
};
};
#endif
