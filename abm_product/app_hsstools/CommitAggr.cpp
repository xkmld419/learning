/*VER: 1*/ 
#include "Process.h"
#include "Environment.h"

class CommitAggr : public Process
{
  public:
	int run();
};

DEFINE_MAIN (CommitAggr)

int CommitAggr::run()
{
	if (!G_PTRANSMGR) {
		G_PTRANSMGR = new TransactionMgr ();
	}

	G_PTRANSMGR->setForceCommit ();
	G_PTRANSMGR->commit ();
	DB_LINK->commit ();

	return 0;
	
}
