/*VER: 1*/ 
#include "Process.h"

class ForceCommit : public Process
{
  public:
	int run()
	{
		SEND_FORCE_COMMIT_EVENT ();
		return 0;
	}
};

DEFINE_MAIN (ForceCommit);

