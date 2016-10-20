/*VER: 1*/ 
#include <stdio.h>
#include <stdlib.h>
#include "Process.h"
#include "Environment.h"
#include "UserInfoCtl.h"
#include "Log.h"
#include "WorkFlow.h"

//#define DEBUG_MODE_NO_DELETE

class GetSHMEvent: public Process
{
	public:
		int run ();
		GetSHMEvent ();
		~GetSHMEvent ();

};

GetSHMEvent::GetSHMEvent ()
{
	Environment::useUserInfo ();
	Environment::useEventSectionUnion ();
}

GetSHMEvent::~GetSHMEvent ()
{

}

int GetSHMEvent::run ()
{
	StdEvent event;
	if (!GetLastEvent (&event)) {
		printf ("no event\n");
		return 0;
	} else {
		cout<< "calling: [" << event.m_sCallingNBR << "]" <<endl;
		cout<< "called: [" << event.m_sCalledNBR << "]" <<endl;
		cout<< "serv_id: [" << event.m_iServID << "]" <<endl;
		cout<< "start_time: [" << event.m_sStartDate << "]" <<endl;
		cout<< "duration: [" << event.m_iDuration << "]" <<endl;
	}
}

DEFINE_MAIN(GetSHMEvent);

