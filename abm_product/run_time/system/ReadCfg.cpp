#include<ReadCfg.h>

ReadCfg::ReadCfg()
{
	m_vTab.clear();
	m_mIndex.clear();
	m_bAttach = false;	
}

ReadCfg::~ReadCfg()
{
	m_vTab.clear();
	m_mIndex.clear();
	m_bAttach = false;	
}

int ReadCfg::_read(ABMException &oExp, char *pSrc, char row[][C_CONTEXT_MAX], char sSign)
{
	if (pSrc == NULL) {
        ADD_EXCEPT0(oExp, "parameter error : pSrc = 0");
		return -1;	
	}		
	
	int len, i = 0;
	char *p = pSrc;
	char *pp;
	
	while ((pp=strchr(p, sSign)) != NULL) {
		if ((len=int(pp - p)) >= C_CONTEXT_MAX) {
            ADD_EXCEPT2(oExp, "data:\"%s\" is too long. lenth must < %d",
                    p, C_CONTEXT_MAX);
			return -1;	
		}

        //20101012. 19:07 xueqt add for trim space
		
        while (*p==' ' || *p=='\t') {
            p++;
            len--;
        }
		strncpy(row[i], p, len);
		row[i][len] = '\0';

        while ((--len)>=0) {
            switch (row[i][len]) {
            case ' ':
            case '\t':
                row[i][len] = 0;
                break;
            default:
                len = 0;
                break;
            }
        }
		
		//p = ++p;
		p=pp+1;
		if (++i > T_COLUMN_MAX) {
			return T_COLUMN_MAX;	
		}				
	}
	
	if ((p[0]!='\n')&&(p[0]!='\0')&& (i<=T_COLUMN_MAX)) {
		if (strlen(p) >= C_CONTEXT_MAX) {
            ADD_EXCEPT2(oExp, "data:\"%s\" is too long. lenth must < %d",
                    p, C_CONTEXT_MAX);
			return -1;	
		}
		if(p[strlen(p)-1]=='\n') //表示最后一行
			strncpy(row[i], p,strlen(p)-1); 	
		else strcpy(row[i], p);
		return i+1;//mofidy by nwp 
	}
	
	return i;
}

int ReadCfg::read(ABMException &oExp, const char *pf, char const *pTag, char sSign)
{
	if ((pf==NULL) || (pTag==NULL)) {
        ADD_EXCEPT2(oExp, "parameter error: pf=%p pTag=%p; cannot be 0", pf, pTag);
		return -1;	
	}	

	FILE *fp = fopen(pf, "r");
    if (fp == NULL) {
        ADD_EXCEPT2(oExp, "fopen(%s) for read failed. ERRNO:%d", pf, errno);
    	return -1;	
    }

    memset (m_sFileName, 0, sizeof(m_sFileName));
    memset (m_sTagName, 0, sizeof(m_sTagName));

    strncpy (m_sTagName, pTag, sizeof(m_sTagName)-1);

    char const *p = pf+strlen(pf)-1;
    //open  成功了，pf不可能为空
    while (*p!='/' && p>=pf) {
        p--;
    }
    if (*p=='/') p++;
    strncpy (m_sFileName, p, sizeof(m_sFileName)-1);
    
    int itotal, ret = 0;
    string index;
    bool start = false;	
	char buf[3072];
	char tname[64];	
	_RowText rowdata;
	
	m_vTab.clear();
	while(!feof(fp)) {
		memset(buf, 0x00, sizeof(buf));
        memset(&rowdata, 0x00, sizeof(rowdata));

		if (fgets(buf, 3072, fp) == NULL) {
			continue;	
		}
		//cout<<strlen(buf)<<endl;
		if (buf[0] == '#') {
			continue;	
		} else if ((buf[0]=='<') && (buf[strlen(buf)-2]=='>')) {
			//modify by nwp 最后一位是\n
			if (start) {
				ret = 0; break;		
			}
			memset(tname, 0x00, sizeof(tname));
			strncpy(tname, &buf[1], strlen(buf)-3);//modify by nwp 最后一位是\n
			tname[sizeof(tname)-1]='\0';
			if (strcmp(tname, pTag) == 0) {
				start = true;		
			}
			continue;	

		} else if (buf[0] == '&') {
			if (!start) {
				continue;		
			}
			if ((itotal=_read(oExp, buf, rowdata.m_sRow, sSign)) == -1) {
                ADD_EXCEPT3(oExp, "[%s]<%s> ReadCfg::_read() failed. Data is :%s", 
                        m_sFileName, m_sTagName, buf);
				ret = -1; break;	
			}
			m_mIndex.clear();
			for (int i=1; i<itotal; ++i) {
				index = rowdata.m_sRow[i];
				m_mIndex.insert(pair<string, int>(index, i));		
			}
			
		} else if (buf[0] == '*') {
			if (!start) {
				continue;		
			}

			if ((itotal=_read(oExp, buf, rowdata.m_sRow, sSign)) == -1) {
                ADD_EXCEPT3(oExp, "[%s]<%s> ReadCfg::_read() failed. Data is :%s", 
                        m_sFileName, m_sTagName, buf);
				ret = -1; break;	
			}

			m_vTab.push_back(rowdata);
		} else {
        		
        }				
	}
	
	fclose(fp);

    if (!start) {
        ADD_EXCEPT2(oExp, "<%s> not found in [%s]", m_sTagName, m_sFileName);
        ret = -1;
    }
	
	return ret;
}
int ReadCfg::getRows()
{
	return m_vTab.size();
}
int ReadCfg::getColumns()
{
	return m_mIndex.size();
}
bool ReadCfg::getValue(ABMException &oExp, char *sValue,int iColumn,int iLine, int iMaxLen)
{
	if(iColumn>m_mIndex.size()||iLine>m_vTab.size()) {
        ADD_EXCEPT6(oExp, "[%s]<%s> column %d row %d has no data. max column is: %d. max row is %d",
                m_sFileName, m_sTagName, iColumn, iLine, m_mIndex.size(), m_vTab.size());
		return false;
    } else {
		m_itr=m_vTab.begin() + iLine - 1;
		strncpy(sValue, m_itr->m_sRow[iColumn], iMaxLen-1);
	}

	return true;
}

bool ReadCfg::getValue(ABMException &oExp, int &iValue,int iColumn,int iLine)
{
	char sValue[C_CONTEXT_MAX];
	if(iColumn>m_mIndex.size()||iLine>m_vTab.size()) {
        ADD_EXCEPT6(oExp, "[%s]<%s> column %d row %d has no data. max column is: %d. max row is %d",
                m_sFileName, m_sTagName, iColumn, iLine, m_mIndex.size(), m_vTab.size());
		return false;
    } else {
		m_itr=m_vTab.begin()+iLine - 1;
		strcpy(sValue, m_itr->m_sRow[iColumn]);
		if(strcmp(sValue,"")==0) iValue =-1;
		else iValue =atoi(sValue);
	}
	return true;
}

bool ReadCfg::getValue(ABMException &oExp, long &lValue,int iColumn,int iLine)
{
	char sValue[C_CONTEXT_MAX];
	if(iColumn>m_mIndex.size()||iLine>m_vTab.size()) {
        ADD_EXCEPT6(oExp, "[%s]<%s> column %d row %d has no data. max column is: %d. max row is %d",
                m_sFileName, m_sTagName, iColumn, iLine, m_mIndex.size(), m_vTab.size());
		return false;
    } else {
		m_itr=m_vTab.begin()+iLine - 1;
		strcpy(sValue, m_itr->m_sRow[iColumn]);
		if(strcmp(sValue,"")==0) lValue =-1;
		else lValue =atol(sValue);
	}
	return true;
}
bool ReadCfg::getValue(ABMException &oExp, char *sValue,char *sColName,int iLine, int iMaxLen)
{
	if(iLine>m_vTab.size()) {
        ADD_EXCEPT5(oExp, "[%s]<%s> column %s row %d has no data. max row is %d",
                m_sFileName, m_sTagName, sColName, iLine, m_vTab.size());
		return false;
    } else {
		string index(sColName);
		map<string, int>::iterator itr = m_mIndex.find(index);
		if(itr==m_mIndex.end())	{
            ADD_EXCEPT3(oExp, "[%s]<%s> column %s not exist",
                    m_sFileName, m_sTagName, sColName);
            return false;
        }
		m_itr=m_vTab.begin()+ iLine- 1;
		strncpy(sValue, m_itr->m_sRow[itr->second], iMaxLen-1);
	}

	return true;
}

bool ReadCfg::getValue(ABMException &oExp, int &iValue,char *sColName,int iLine)
{
	char sValue[C_CONTEXT_MAX];
	if(iLine>m_vTab.size()) {
        ADD_EXCEPT5(oExp, "[%s]<%s> column %s row %d has no data. max row is %d",
                m_sFileName, m_sTagName, sColName, iLine, m_vTab.size());
		return false;
    } else {
		string index(sColName);
		map<string, int>::iterator itr = m_mIndex.find(index);
		if(itr==m_mIndex.end())	{
            ADD_EXCEPT3(oExp, "[%s]<%s> column %s not exist",
                    m_sFileName, m_sTagName, sColName);
            return false;
        }
		m_itr=m_vTab.begin()+ iLine- 1;
		strcpy(sValue, m_itr->m_sRow[itr->second]);
		if(strcmp(sValue,"")==0) 
			iValue = -1;
		else iValue =atoi(sValue);
	}

	return true;
}

bool ReadCfg::getValue(ABMException &oExp, long &lValue,char *sColName,int iLine)
{
	char sValue[C_CONTEXT_MAX];
	if(iLine>m_vTab.size()) {
        ADD_EXCEPT5(oExp, "[%s]<%s> column %s row %d has no data. max row is %d",
                m_sFileName, m_sTagName, sColName, iLine, m_vTab.size());
		return false;
    } else {
		string index(sColName);
		map<string, int>::iterator itr = m_mIndex.find(index);
		if(itr==m_mIndex.end())	{
            ADD_EXCEPT3(oExp, "[%s]<%s> column %s not exist",
                    m_sFileName, m_sTagName, sColName);
            return false;
        }
		m_itr=m_vTab.begin()+ iLine- 1;
		strcpy(sValue, m_itr->m_sRow[itr->second]);
		if(strcmp(sValue,"")==0) 
			lValue = -1;
		else lValue =atol(sValue);
	}

	return true;
}

