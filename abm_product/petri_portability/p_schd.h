#include "petri.h"

namespace PETRI {


struct schd {

	schd() {
		mypid = getpid();
		l_wsnID = -1;
		m_lActionID = -1;
	};
	void setWSNID(long wsn_id = -1) {l_wsnID = wsn_id;}

	void doschd();
	void sendout();
	void collect();
	void collect1();
	long getLatestID();
	inline void state();





private:
	pid_t mypid;
	long m_lActionID;
	vector<unsigned long> transList;
	vector<unsigned long> transList1;
	long l_wsnID;


};





}

