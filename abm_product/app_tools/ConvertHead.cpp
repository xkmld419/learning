/*VER: 1*/ 
/*
**	导号头
**	Zhui
*/

#include "Process.h"
#include "Stack.h"

class KeyNode
{
  public:

	KeyNode * m_poParent;
	KeyNode * m_poChild[10];

	int m_iOrgID;

	KeyNode () {
		m_poParent = 0;
		memset (m_poChild, 0, sizeof (m_poChild));
		m_iOrgID = 0;
	}

	KeyNode (KeyNode * pParent) {
		m_poParent = pParent;
		memset (m_poChild, 0, sizeof (m_poChild));
		m_iOrgID = 0;
	}

	bool equal(int iOrgID, KeyNode ** ppNode) {
		if (m_iOrgID != iOrgID) {
			*ppNode = (KeyNode *)this;
			return false;
		}

		int iTemp;

		for (iTemp=0; iTemp<10; iTemp++) {
			if (m_poChild[iTemp]) {
				m_sTemp[m_iTemp] = iTemp | 0X30;
				m_iTemp++;

				if (!m_poChild[iTemp]->equal (iOrgID)) return false;
	
				m_sTemp[m_iTemp] = 0;
				m_iTemp--;
			}
		}

		return true;
	}


};

class ConvertHead : public Process
{
  public:
	int run();
	ConvertHead()


  private:
	int loadData ();
	int writeData ();
	int insertData (char *sKey, int iOrg);

  private:
	KeyNode m_poHead[1];
	char m_sTemp[12];
	int m_iTemp;

};

int ConvertHead::writeData(KeyNode * pNode)
{
	KeyNode * pNode1;

	while (!pNode->equal (pNode->m_iOrgID, &pNode1)) {
		writeData (pNode1);	
	}

	if (pNode->equal (pNode->m_iOrgID, &pNode1)) {
		writeDB ( m_sTemp, pNode->m_iOrgID);
		pNode1 = pNode->m_poParent;
		int iTemp = m_sTemp[m_iTemp] & 0X30;
		pNode1->m_poChild[iTemp] = 0;

		m_sTemp[m_iTemp] = 0;
		m_iTemp--;

		return 0;
	}


}
	

int ConvertHead::insertData(char *sKey, int iOrg)
{
	KeyNode * pNode = &(m_poHead[0]);
	char * pTemp = sKey;
	int iTemp;

	while (*pTemp) {
		iTemp = ((*pTemp) & 0X30);

		if (!pNode->m_iChild[iTemp]) {
			pNode->m_iChild[iTemp] = new KeyNode (pNode);
			if (!(pNode->m_iChild[iTemp])) THROW (1);
		}

/*
		if (!pNode->m_iOrgID) {
			pNode->m_iOrgID = iOrg;
		}
*/

		pNode = pNode->m_iChild[iTemp];

		pTemp++;
	}

	pNode->m_iOrgID = iOrg;
	
	return 0;
}

int ConvertHead::loadData()
{
	char sMin[12], sMax[12], sTemp[12];
	int iMin, iMax, iOrg, iTemp;

	DEFINE_QUERY (qry);
	qry.setSQL ("select min_nbr, max_nbr, org_id from km_head");
	qry.open ();

	while (qry.next ()) {
		strcpy (sMin, qry.field(0).asString ());
		strcpy (sMax, qry.field(1).asString ());
		iOrg = qry.field(2).asInteger ();

		/*	不可能字符 ， 不可能0开头	*/
		iMin = strtol (sMin, 0, 10);
		iMax = strtol (sMax, 0, 10);

		for (iTemp=iMin; iTemp<=iMax; iTemp++) {
			sprintf (sTemp, "%d\0", iTemp);
			insertData (sTemp, iOrg);
		}
		
	}


	return 0;
}

