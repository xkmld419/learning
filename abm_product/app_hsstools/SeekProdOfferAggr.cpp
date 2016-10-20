/*VER: 1*/ 
#include "ProdOfferAggr.h"
#include "Process.h"

int const total      = 1;
int const detail     = 2;


class SeekOfferAggr 
{
  public:
	int run();
	SeekOfferAggr()  {
		m_iCommand = 0;
		m_lItemDisctSeq = 0;
		m_lProdOfferID = 0;
	}
  private:
	void prepare();
	int  m_iCommand;
	long m_lItemDisctSeq;
	long m_lProdOfferID;
};

int main(int argc, char *argv[])
{
	g_argc = argc;
	g_argv = argv;

	SeekOfferAggr x;

	return x.run ();
}


void SeekOfferAggr::prepare()
{
    int i = 1;

    while (i < g_argc) {
        if (g_argv[i][0] != '-') {
            i++;
            continue;
        }

        switch (g_argv[i][1]) {
        case 't':
            m_iCommand = total;
            i++;
            break;
        case 'd':
        	
            m_iCommand = detail;
            i++;
            break;
        case 'o':
        	i++;
            if (i<g_argc)
                m_lProdOfferID = atol (g_argv[i]);
            i++;
            break;
        case 's':
        	i++;
            if (i<g_argc)
                m_lItemDisctSeq = atol (g_argv[i]);
            break;

        default:
            i++;
            break;
        }
    }
    
    return;
}
int SeekOfferAggr::run()
{
	prepare ();

    ProdOfferDisctMgr ctl;
    
    switch (m_iCommand) {
    	case total:
    		ctl.GetOfferAggrRec(m_lProdOfferID);
    		break;
    	case detail:
    		ctl.GetOfferAggrDetail(m_lItemDisctSeq,m_lProdOfferID);
    		break;
    	default:
    		cout << "seekofferaggr -t -o ProdOfferID" << endl;
    		cout << "seekofferaggr -d -o ProdOfferID -s ItemDisctSeq" << endl;
    		break;	
    }

	return 0;
}
