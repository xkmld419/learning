/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#include "SHMAccess.h"
#include <string.h>

#include "MBC.h"

//##ModelId=42442817022A
SHMAccess::SHMAccess(ABMException &oExp,char *shmname) :
m_piSize (0), m_poSHM (0),m_iError(0), m_lSHMKey (0)
{
	m_sSHMName = new char[strlen(shmname)+1];
	if (!m_sSHMName)
		THROW(MBC_SHMAccess+1);
	strcpy (m_sSHMName, shmname);
	open (oExp);
}


//##ModelId=424428170248
SHMAccess::SHMAccess(ABMException &oExp,long shmkey) :
m_piSize (0), m_poSHM (0), m_sSHMName(0),m_iError(0), m_lSHMKey (shmkey)
{
	open (oExp);
}


//##ModelId=424428170266
SHMAccess::~SHMAccess()
{
	if (m_sSHMName)
		delete [] m_sSHMName;

    if (m_poSHM) {
        delete m_poSHM;
        m_poSHM = 0;
    }
}

//##ModelId=424428BF005F
bool SHMAccess::exist()
{
	return m_poSHM;
}

//##ModelId=424428CB0099
void SHMAccess::remove(ABMException &oExp)
{
	m_poSHM->remove (oExp);
	delete m_poSHM;
	m_poSHM = 0;
}

//##ModelId=4244C70600D3
void SHMAccess::open(ABMException &oExp)
{
	unsigned long size; 
	//if (m_sSHMName) {
	//	m_poSHM = new SimpleSHM (m_sSHMName, 4);//此处只是给m_poSHM的几个数据成员赋值
	//} else {
		m_poSHM = new SimpleSHM (m_lSHMKey, 4); //此处只是给m_poSHM的几个数据成员赋值
	//}

	if (!m_poSHM)
		THROW(MBC_SHMAccess+2);

	if (m_poSHM->exist(oExp)) {
		m_iError = m_poSHM->open (oExp,false,0777);

        if (m_iError == 0 ) {

            size = m_poSHM->size ();
            m_poSHM->close ();
            delete m_poSHM;
            m_poSHM = 0;
            m_poSHM = new SimpleSHM (m_lSHMKey, size);

            if (!m_poSHM)
                THROW(MBC_SHMAccess+2);
            m_iError = m_poSHM->open (oExp,false,0777);
        }

	} else {
		delete m_poSHM;
		m_poSHM = 0;
	}		
}

//##ModelId=4244CAB6013E
void SHMAccess::create(ABMException &oExp,unsigned long size, unsigned int iPermission)
{
	//if (m_sSHMName) {
	//	m_poSHM = new SimpleSHM (m_sSHMName, size);
	//} else {
		m_poSHM = new SimpleSHM (m_lSHMKey, size);
	//}

	if (!m_poSHM)
		THROW(MBC_SHMAccess+2);
	
	m_iError = m_poSHM->open (oExp,true,iPermission);

    if (m_iError != 0 ) {
        return ;
    }

	memset(m_poSHM->getPointer(), 0, size);
}

int SHMAccess::connnum()
{
	if(m_poSHM)
		return m_poSHM->nattch();
	else
		return 0;
}
