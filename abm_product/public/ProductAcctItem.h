/*VER: 1*/ 
#ifndef PRODUCT_ACCT_ITEM_H
#define PRODUCT_ACCT_ITEM_H

#include "SimpleMatrix.h"
#include "HashList.h"

class ProductAcctItem
{
  public:
	ProductAcctItem();

  public:
	bool check(int iProductID, int iAcctItemTypeID);

  public:
	void load();
	void unload();

  private:
	void add(HashList<int> *pList, int iAcctItemTypeID);

  private:
	static HashList<HashList<int> *> * m_poChecker;
	static bool m_bUploaded;
};

#endif
