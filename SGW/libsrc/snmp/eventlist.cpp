/*_############################################################################
  _## 
  _##  eventlist.cpp  
  _##
  _##  SNMP++v3.2.22
  _##  -----------------------------------------------
  _##  Copyright (c) 2001-2007 Jochen Katz, Frank Fock
  _##
  _##  This software is based on SNMP++2.6 from Hewlett Packard:
  _##  
  _##    Copyright (c) 1996
  _##    Hewlett-Packard Company
  _##  
  _##  ATTENTION: USE OF THIS SOFTWARE IS SUBJECT TO THE FOLLOWING TERMS.
  _##  Permission to use, copy, modify, distribute and/or sell this software 
  _##  and/or its documentation is hereby granted without fee. User agrees 
  _##  to display the above copyright notice and this license notice in all 
  _##  copies of the software and any documentation of the software. User 
  _##  agrees to assume all liability for the use of the software; 
  _##  Hewlett-Packard and Jochen Katz make no representations about the 
  _##  suitability of this software for any purpose. It is provided 
  _##  "AS-IS" without warranty of any kind, either express or implied. User 
  _##  hereby grants a royalty-free license to any and all derivatives based
  _##  upon this software code base. 
  _##  
  _##  Stuttgart, Germany, Wed May  2 23:22:30 CEST 2007 
  _##  
  _##########################################################################*/
/*===================================================================

  Copyright (c) 1999
  Hewlett-Packard Company

  ATTENTION: USE OF THIS SOFTWARE IS SUBJECT TO THE FOLLOWING TERMS.
  Permission to use, copy, modify, distribute and/or sell this software
  and/or its documentation is hereby granted without fee. User agrees
  to display the above copyright notice and this license notice in all
  copies of the software and any documentation of the software. User
  agrees to assume all liability for the use of the software; Hewlett-Packard
  makes no representations about the suitability of this software for any
  purpose. It is provided "AS-IS" without warranty of any kind,either express
  or implied. User hereby grants a royalty-free license to any and all
  derivatives based upon this software code base.

      E V E N T L I S T . C P P

      CEventList  CLASS DEFINITION

      COPYRIGHT HEWLETT PACKARD COMPANY 1999

      INFORMATION NETWORKS DIVISION

      NETWORK MANAGEMENT SECTION


      DESIGN + AUTHOR:
        Tom Murray

      LANGUAGE:
        ANSI C++

      OPERATING SYSTEMS:
        DOS/WINDOWS 3.1
        BSD UNIX

      DESCRIPTION:
        Queue for holding all event sources (snmp messages, user
        defined input sources, user defined timeouts, etc)

      COMPILER DIRECTIVES:
        UNIX - For UNIX build

=====================================================================*/
char event_list_version[]="@(#) SNMP++ $Id: eventlist.cpp 232 2006-03-10 20:43:44Z katz $";

//----[ snmp++ includes ]----------------------------------------------

#include "snmp_pp/config_snmp_pp.h"
#include "snmp_pp/v3.h"
#include "snmp_pp/eventlist.h"		// queue for holding all event sources
#include "snmp_pp/msgqueue.h"		// queue for holding snmp event sources
#include "snmp_pp/notifyqueue.h"	// queue for holding trap callbacks
#include "snmp_pp/snmperrs.h"

#ifdef SNMP_PP_NAMESPACE
namespace Snmp_pp {
#endif

//----[ CSNMPMessageQueueElt class ]--------------------------------------

CEventList::CEventListElt::CEventListElt(CEvents *events,
					 CEventListElt *next,
					 CEventListElt *previous):
  m_events(events), m_Next(next), m_previous(previous)
{
  /* Finish insertion into doubly linked list */
  if (m_Next)     m_Next->m_previous = this;
  if (m_previous) m_previous->m_Next = this;
}

CEventList::CEventListElt::~CEventListElt()
{
  /* Do deletion form doubly linked list */
  if (m_Next)     m_Next->m_previous = m_previous;
  if (m_previous) m_previous->m_Next = m_Next;
  if (m_events)   delete m_events;
}


//----[ CEventList class ]--------------------------------------


CEventList::~CEventList()
{
  CEventListElt *leftOver;

  /* walk the list deleting any elements still on the queue */
  lock();
  while ((leftOver = m_head.GetNext()))
    delete leftOver;
  unlock();
}

CEvents * CEventList::AddEntry(CEvents *events) REENTRANT ({
    /*---------------------------------------------------------*/
    /* Insert entry at head of list, done automagically by the */
    /* constructor function, so don't use the return value.    */
    /*---------------------------------------------------------*/
  (void) new CEventListElt(events, m_head.GetNext(), &m_head);
  m_msgCount++;

  return events;
})

int CEventList::GetNextTimeout(msec &sendTime) REENTRANT ({

  CEventListElt *msgEltPtr = m_head.GetNext();
  msec tmpTime(sendTime);

  sendTime.SetInfinite();	// set sendtime out into future
  while (msgEltPtr) {
    if (msgEltPtr->GetEvents()->GetCount() &&
	!msgEltPtr->GetEvents()->GetNextTimeout(tmpTime)) {
      if (sendTime > tmpTime)
	sendTime = tmpTime;
    }

    msgEltPtr = msgEltPtr->GetNext();
  }
 return 0;
})


void CEventList::GetFdSets(int &maxfds, fd_set &readfds, fd_set &writefds,
			   fd_set &exceptfds) REENTRANT ({

  CEventListElt *msgEltPtr = m_head.GetNext();

  maxfds = 0;
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
  while (msgEltPtr) {
    if (msgEltPtr->GetEvents()->GetCount()) {
      msgEltPtr->GetEvents()->GetFdSets(maxfds, readfds, writefds, exceptfds);
    }
    msgEltPtr = msgEltPtr->GetNext();
  }
})

int CEventList::HandleEvents(const int maxfds,
			     const fd_set &readfds,
			     const fd_set &writefds,
			     const fd_set &exceptfds)
{
  lock();
  CEventListElt *msgEltPtr = m_head.GetNext();
  int status = SNMP_CLASS_SUCCESS;
  while (msgEltPtr){
    if (msgEltPtr->GetEvents()->GetCount()) {
      unlock();
      status = msgEltPtr->GetEvents()->HandleEvents(maxfds, readfds, writefds,
						    exceptfds);
      lock();
    }
    msgEltPtr = msgEltPtr->GetNext();
  }
  unlock();
  return status;
}

int CEventList::DoRetries(const msec &sendtime) REENTRANT ({

  CEventListElt *msgEltPtr = m_head.GetNext();
  int status = SNMP_CLASS_SUCCESS;
  while (msgEltPtr){
    if (msgEltPtr->GetEvents()->GetCount()) {
      status = msgEltPtr->GetEvents()->DoRetries(sendtime);
    }
    msgEltPtr = msgEltPtr->GetNext();
  }
  return status;
})

int CEventList::Done() REENTRANT ({

  CEventListElt *msgEltPtr = m_head.GetNext();
  int status = SNMP_CLASS_SUCCESS;

  if (m_done) {
    m_done--;
    return 1;
  }

  while (msgEltPtr){
    if (msgEltPtr->GetEvents()->GetCount()) {
      status = msgEltPtr->GetEvents()->Done();
      if (status)
	break;
    }
    msgEltPtr = msgEltPtr->GetNext();
  }
  return status;
})

#ifdef SNMP_PP_NAMESPACE
}; // end of namespace Snmp_pp
#endif 
