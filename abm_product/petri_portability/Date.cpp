/*VER: 3*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "Date.h"
//#include "Exception.h"
#include <time.h>

//#include "MBC.h"

char Date::m_sDateMin[16][9] =
{
    "19450101","0000101","000101","00101","0101","001","01","0"
        ,"19450101","0000101","000101","00101","0101","101","01",
        "1"
};


short Date::m_iDaysSumOfOneYear[2][14] =
{
	{0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
	{0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
};

short Date::m_iDaysOfOneYear[2][13] =
{
	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

int Date::m_iYearsSumDays[401] = 
{
	0,366,731,1096,1461,1827,2192,2557,
	2922,3288,3653,4018,4383,4749,5114,5479,
	5844,6210,6575,6940,7305,7671,8036,8401,
	8766,9132,9497,9862,10227,10593,10958,11323,
	11688,12054,12419,12784,13149,13515,13880,14245,
	14610,14976,15341,15706,16071,16437,16802,17167,
	17532,17898,18263,18628,18993,19359,19724,20089,
	20454,20820,21185,21550,21915,22281,22646,23011,
	23376,23742,24107,24472,24837,25203,25568,25933,
	26298,26664,27029,27394,27759,28125,28490,28855,
	29220,29586,29951,30316,30681,31047,31412,31777,
	32142,32508,32873,33238,33603,33969,34334,34699,
	35064,35430,35795,36160,36525,36890,37255,37620,
	37985,38351,38716,39081,39446,39812,40177,40542,
	40907,41273,41638,42003,42368,42734,43099,43464,
	43829,44195,44560,44925,45290,45656,46021,46386,
	46751,47117,47482,47847,48212,48578,48943,49308,
	49673,50039,50404,50769,51134,51500,51865,52230,
	52595,52961,53326,53691,54056,54422,54787,55152,
	55517,55883,56248,56613,56978,57344,57709,58074,
	58439,58805,59170,59535,59900,60266,60631,60996,
	61361,61727,62092,62457,62822,63188,63553,63918,
	64283,64649,65014,65379,65744,66110,66475,66840,
	67205,67571,67936,68301,68666,69032,69397,69762,
	70127,70493,70858,71223,71588,71954,72319,72684,
	73049,73414,73779,74144,74509,74875,75240,75605,
	75970,76336,76701,77066,77431,77797,78162,78527,
	78892,79258,79623,79988,80353,80719,81084,81449,
	81814,82180,82545,82910,83275,83641,84006,84371,
	84736,85102,85467,85832,86197,86563,86928,87293,
	87658,88024,88389,88754,89119,89485,89850,90215,
	90580,90946,91311,91676,92041,92407,92772,93137,
	93502,93868,94233,94598,94963,95329,95694,96059,
	96424,96790,97155,97520,97885,98251,98616,98981,
	99346,99712,100077,100442,100807,101173,101538,101903,
	102268,102634,102999,103364,103729,104095,104460,104825,
	105190,105556,105921,106286,106651,107017,107382,107747,
	108112,108478,108843,109208,109573,109938,110303,110668,
	111033,111399,111764,112129,112494,112860,113225,113590,
	113955,114321,114686,115051,115416,115782,116147,116512,
	116877,117243,117608,117973,118338,118704,119069,119434,
	119799,120165,120530,120895,121260,121626,121991,122356,
	122721,123087,123452,123817,124182,124548,124913,125278,
	125643,126009,126374,126739,127104,127470,127835,128200,
	128565,128931,129296,129661,130026,130392,130757,131122,
	131487,131853,132218,132583,132948,133314,133679,134044,
	134409,134775,135140,135505,135870,136236,136601,136966,
	137331,137697,138062,138427,138792,139158,139523,139888,
	140253,140619,140984,141349,141714,142080,142445,142810,
	143175,143541,143906,144271,144636,145002,145367,145732,
	146097	
};

char Date::m_iYearsDays[400] = 
{
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,0,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	0,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,0,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0,
	1,0,0,0,1,0,0,0
};
//##ModelId=41DCA1BB012A
Date::Date()
{
	getCurDate ();
}

int Date::operator- (const Date &right)
{
	stdToInner ();

	Date d = right;
	d.stdToInner ();

	return m_iDays - d.m_iDays;
}

int Date::diffStrictYear(Date &right)
{
	int iRet = m_iYear - right.m_iYear;
	if ((m_iMon*31+m_iDay) < (right.m_iMon*31+right.m_iDay)) {
		iRet--;
	}

	return iRet;
}

int Date::diffStrictMon(Date &right)
{
	int iRet = m_iYear*12 + m_iMon - right.m_iYear*12 - right.m_iMon;
	if (m_iDay < right.m_iDay) {
		iRet--;
	}

	return iRet;
}

int Date::diffStrictDay(Date &right)
{
	stdToInner ();

	Date d = right;
	d.stdToInner ();

	int iRet = m_iDays - d.m_iDays;
	
	return iRet;
}

int Date::diffMon(Date &right)
{
	return (right.m_iYear * 12 + right.m_iMon - this->m_iYear * 12 - this->m_iMon);
}

long Date::diffSec(Date &right)
{
	stdToInner ();
	stdToInnerA ();

	right.stdToInner ();
	right.stdToInnerA ();

	return (m_iDays-right.m_iDays)*86400 + (m_iSeconds-right.m_iSeconds);
}

//##ModelId=41E76D9A00B9
Date::Date(char const *sDateStr, char const *sFormat)
{
	if (!parse(sDateStr, sFormat)) 
//	THROW(MBC_Date+1);
	exit(-1);
}

//##ModelId=41E76D8503C6
void Date::getCurDate()
{
	struct timeval tv;
	struct timezone tz;
	struct tm * p;

	int i = gettimeofday (&tv , &tz);

	if (i == -1) 
//	THROW(MBC_Date+2);
	exit(-1);
	
	p = localtime (&(tv.tv_sec));

	m_iYear = 300 + p->tm_year;
	m_iMon = p->tm_mon + 1;
	m_iDay = p->tm_mday;
	m_iHour = p->tm_hour;
	m_iMin = p->tm_min;
	m_iSec = p->tm_sec;
	m_iMilSec = tv.tv_usec;
}

//##ModelId=41DCA1BB013E
Date::~Date()
{
}

//##ModelId=41DCA2210267
bool Date::operator==(const Date& right)
{
	return (m_iYear == right.m_iYear &&
		m_iMon == right.m_iMon &&
		m_iDay == right.m_iDay &&
		m_iHour == right.m_iHour &&
		m_iMin == right.m_iMin &&
		m_iSec == right.m_iSec);
}

//##ModelId=41DCA22102B7
bool Date::operator!=(const Date& right)
{
	return (m_iYear != right.m_iYear ||
		m_iMon != right.m_iMon ||
		m_iDay != right.m_iDay ||
		m_iHour != right.m_iHour ||
		m_iMin != right.m_iMin ||
		m_iSec != right.m_iSec);
}

//##ModelId=41DCA2210312
bool Date::operator<(const Date& right)
{
	if (m_iYear < right.m_iYear) return true;
	if (m_iYear > right.m_iYear) return false;

	if (m_iMon < right.m_iMon) return true;
	if (m_iMon > right.m_iMon) return false;
	
	if (m_iDay < right.m_iDay) return true;
	if (m_iDay > right.m_iDay) return false;

	if (m_iHour < right.m_iHour) return true;
	if (m_iHour > right.m_iHour) return false;

	if (m_iMin < right.m_iMin) return true;
	if (m_iMin > right.m_iMin) return false;
	
	if (m_iSec < right.m_iSec) return true;

	return false;
}

//##ModelId=41DCA221036C
bool Date::operator<=(const Date& right)
{
	if (m_iYear < right.m_iYear) return true;
	if (m_iYear > right.m_iYear) return false;

	if (m_iMon < right.m_iMon) return true;
	if (m_iMon > right.m_iMon) return false;
	
	if (m_iDay < right.m_iDay) return true;
	if (m_iDay > right.m_iDay) return false;

	if (m_iHour < right.m_iHour) return true;
	if (m_iHour > right.m_iHour) return false;

	if (m_iMin < right.m_iMin) return true;
	if (m_iMin > right.m_iMin) return false;
	
	if (m_iSec > right.m_iSec) return false;

	return true;
}

//##ModelId=41DCA22103A8
bool Date::operator>(const Date& right)
{
	if (m_iYear > right.m_iYear) return true;
	if (m_iYear < right.m_iYear) return false;

	if (m_iMon > right.m_iMon) return true;
	if (m_iMon < right.m_iMon) return false;
	
	if (m_iDay > right.m_iDay) return true;
	if (m_iDay < right.m_iDay) return false;

	if (m_iHour > right.m_iHour) return true;
	if (m_iHour < right.m_iHour) return false;

	if (m_iMin > right.m_iMin) return true;
	if (m_iMin < right.m_iMin) return false;
	
	if (m_iSec > right.m_iSec) return true;

	return false;
}

//##ModelId=41DCA22103DA
bool Date::operator>=(const Date& right)
{
	if (m_iYear > right.m_iYear) return true;
	if (m_iYear < right.m_iYear) return false;

	if (m_iMon > right.m_iMon) return true;
	if (m_iMon < right.m_iMon) return false;
	
	if (m_iDay > right.m_iDay) return true;
	if (m_iDay < right.m_iDay) return false;

	if (m_iHour > right.m_iHour) return true;
	if (m_iHour < right.m_iHour) return false;

	if (m_iMin > right.m_iMin) return true;
	if (m_iMin < right.m_iMin) return false;
	
	if (m_iSec < right.m_iSec) return false;

	return true;
}

/*
//##ModelId=41DCA6920118
int Date::getDay()
{
    return m_iDay;
}
*/


//##ModelId=41DCA6920154
void Date::setDay(int iDay)
{
    m_iDay = iDay;
}


/*
//##ModelId=41DCA69201EA
int Date::getHour()
{
    return m_iHour;
}
*/


//##ModelId=41DCA6920226
void Date::setHour(int iHour)
{
    m_iHour = iHour;
}


//##ModelId=41DCA692026C
int Date::getMilSec()
{
    return m_iMilSec;
}


//##ModelId=41DCA6920294
void Date::setMilSec(int iMilSec)
{
    m_iMilSec = iMilSec;
}


/*
//##ModelId=41DCA69202DA
int Date::getMin()
{
    return m_iMin;
}
*/


//##ModelId=41DCA6920303
void Date::setMin(int iMin)
{
    m_iMin = iMin;
}


/*
//##ModelId=41DCA6920353
int Date::getMonth()
{
    return m_iMon;
}
*/


//##ModelId=41DCA692037B
void Date::setMonth(int iMonth)
{
    m_iMon = iMonth;
}


//##ModelId=41DCA69203D5
int Date::getSec()
{
    return m_iSec;
}


//##ModelId=41DCA6930015
void Date::setSec(int iSec)
{
    m_iSec = iSec;
}


//##ModelId=41DCA6930079
int Date::getWeek()
{
	stdToInner ();
	
	return ((m_iDays-2) % 7) + 1;
}

/*
//##ModelId=41DCA6930105
int Date::getYear()
{
    return m_iYear + 1600;
}
*/


//##ModelId=41DCA6930141
void Date::setYear(int iYear)
{
    m_iYear = iYear - 1600;
}


//##ModelId=41DCA714036E
char *Date::toString(char * sFormat)
{
	if (!sFormat) {
		sprintf (m_sOutBuf, "%04d%02d%02d%02d%02d%02d",
			m_iYear+1600, m_iMon, m_iDay,
			m_iHour, m_iMin, m_iSec);
	} else {
		char format[32];
		int i;
		char *p;
		char temp[5];

		memset (format, 0, sizeof(format));
		memset (m_sOutBuf, 0, sizeof(m_sOutBuf));
		strncpy (m_sOutBuf, sFormat, sizeof(m_sOutBuf));
		strncpy (format, sFormat, sizeof(format));

		//##小写变大写
		i = strlen(format);
		while (i) {
			i--;
			format[i] |= 0X20;
		}
	
		p = strstr (format, "yyyy");
		if (p) {
			sprintf(temp, "%04d", m_iYear+1600);
			memcpy (m_sOutBuf + (p - format), temp, 4);
		}

		p = strstr (format, "mm");
		if (p) {
			sprintf(temp, "%02d", m_iMon);
			memcpy (m_sOutBuf + (p - format), temp, 2);
		}

		p = strstr (format, "dd");
		if (p) {
			sprintf(temp, "%02d", m_iDay);
			memcpy (m_sOutBuf + (p - format), temp, 2);
		}

		p = strstr (format, "hh");
		if (p) {
			sprintf(temp, "%02d", m_iHour);
			memcpy (m_sOutBuf + (p - format), temp, 2);
		}

		p = strstr (format, "mi");
		if (p) {
			sprintf(temp, "%02d", m_iMin);
			memcpy (m_sOutBuf + (p - format), temp, 2);
		}

		p = strstr (format, "ss");
		if (p) {
			sprintf(temp, "%02d", m_iSec);
			memcpy (m_sOutBuf + (p - format), temp, 2);
		}

	}

	return m_sOutBuf;
}

Date::operator char * ()
{
	sprintf (m_sOutBuf, "%04d%02d%02d%02d%02d%02d",
		m_iYear+1600, m_iMon, m_iDay,
		m_iHour, m_iMin, m_iSec);
	return m_sOutBuf;
}	

//##ModelId=41DCA71C02F7
void Date::addDay(int iDay)
{
	int i;
	i = m_iDay + iDay;
	if (i > m_iDaysOfOneYear[(int)(m_iYearsDays[m_iYear%400])][m_iMon] 
	|| i < 1 ) {
		//##需要重新计算
		stdToInner ();
		m_iDays += iDay;
		innerToStd ();
	} else {
		m_iDay = i;
	} 
}

//##ModelId=41DCA7360222
void Date::addYear(int iYear)
{
	m_iYear += iYear;	
}
void Date::addYearAsOracle(int iYear)
{
    bool bLastDay = false;
    if(m_iDaysOfOneYear[m_iYearsDays[m_iYear%400]][m_iMon] == m_iDay)
        bLastDay = true;

    m_iYear += iYear;	
    if(bLastDay)
        m_iDay = m_iDaysOfOneYear[m_iYearsDays[m_iYear%400]][m_iMon];
}
//##ModelId=41DCA73D02E0
void Date::addSec(int iSec)
{
	m_iSec += iSec;
	if (m_iSec > 59 || m_iSec < 0) {
		stdToInnerA ();
		innerToStdA ();
	}

}

//##ModelId=41DCA743004A
void Date::addMilSec(int iMidSec)
{
	m_iMilSec += iMidSec;
}

//##ModelId=41DCA7490324
void Date::addMin(int iMin)
{
	m_iMin += iMin;
	if (m_iMin > 59 || m_iMin < 0) {
		stdToInnerA ();
		innerToStdA ();
	}
}

//##ModelId=41DCA77F015F
void Date::addMonth(int iMonth)
{
	m_iMon += iMonth;
	stdToInner ();
	innerToStd ();
}

void Date::addMonthAsOracle(int iMonths)
{
    bool bLastDay = false;
    if(m_iDaysOfOneYear[m_iYearsDays[m_iYear%400]][m_iMon] == m_iDay)
        bLastDay = true;
    m_iMon += iMonths;
    stdToInnerF ();
    innerToStd ();
    if(bLastDay)
        m_iDay = m_iDaysOfOneYear[m_iYearsDays[m_iYear%400]][m_iMon];
}
//##ModelId=41DCA78400DA
void Date::addHour(int iHour)
{
	m_iHour += iHour;
	if (m_iHour < 0 || m_iHour > 23) {
		stdToInnerA ();
		innerToStdA ();
	}
}

//##ModelId=41DCA7A10013
int Date::getDayNumInMonth(int iMonth)
{
	int i = m_iYear % 400;
	return m_iDaysOfOneYear[(int)(m_iYearsDays[i])][iMonth];
}

//##ModelId=41DCA7BB01B5
bool Date::isLeapYear(int iYear)
{
	int i;
	if (!iYear) iYear = m_iYear;
	else	iYear -= 1600;

	i = iYear % 400;
	return m_iYearsDays[i];
}

//##ModelId=41DCA7CE00EA
bool Date::parse(char const *sDateStr, char const *sFormat)
{
	//##只认YYYY MM DD HH MI SS
	int i;
	char * p;
	char format[32];
	char temp[5];

	m_iYear = 0;
	m_iMon = 0;
	m_iDay = 0;
	m_iHour = 0;
	m_iMin = 0;
	m_iSec = 0;

	int iyear, imon, iday, ihour, imin, isec;
	if(!sDateStr){
		return false;
	}
	if (!sFormat) {
		//##默认的为YYYYMMDDHH24MISS
		char tempDate[16] = {0};
		strcpy (tempDate, sDateStr);
		m_iSec = atoi (tempDate+12);
		tempDate[12] = 0;
		m_iMin = atoi (tempDate+10);
		tempDate[10] = 0;
		m_iHour = atoi (tempDate+8);
		tempDate[8] = 0;
		m_iDay = atoi (tempDate+6);
		tempDate[6] = 0;
		m_iMon = atoi (tempDate+4);
		tempDate[4] = 0;
		m_iYear = atoi (tempDate) - 1600;
		
		return true;
	}

	memset (format, 0, sizeof(format));
	strncpy (format, sFormat, 31);

	i = strlen(format);
	while (i) {
		i--;
		format[i] |= 0X20;
	}

	p = strstr (format, "yyyy");
	if (p) {
		strncpy (temp, sDateStr+(p-format), 4);
		temp[4] = 0;
		p = 0;
		m_iYear = strtol (temp, &p, 10) - 1600;
		if (*p) return false;
	}

	p = strstr (format, "mm");
	if (p) {
		strncpy (temp, sDateStr+(p-format), 2);
		temp[2] = 0;
		p = 0;
		m_iMon = strtol (temp, &p, 10);
		if (*p) return false;
	}

	p = strstr (format, "dd");
	if (p) {
		strncpy (temp, sDateStr+(p-format), 2);
		temp[2] = 0;
		p = 0;
		m_iDay = strtol (temp, &p, 10);
		if (*p) return false;
	}

	p = strstr (format, "hh");
	if (p) {
		strncpy (temp, sDateStr+(p-format), 2);
		temp[2] = 0;
		p = 0;
		m_iHour = strtol (temp, &p, 10);
		if (*p) return false;
	}

	p = strstr (format, "mi");
	if (p) {
		strncpy (temp, sDateStr+(p-format), 2);
		temp[2] = 0;
		p = 0;
		m_iMin = strtol (temp, &p, 10);
		if (*p) return false;
	}

	p = strstr (format, "ss");
	if (p) {
		strncpy (temp, sDateStr+(p-format), 2);
		temp[2] = 0;
		p = 0;
		m_iSec = strtol (temp, &p, 10);
		if (*p) return false;
	}

/*
	m_iYear = iyear - 1600;
	m_iMon = imon;
	m_iDay = iday;
	m_iHour = ihour;
	m_iMin = imin;
	m_iSec = isec;
*/

	return true;
}

void Date::parseToDay(char const *sDateStr)
{
	m_iYear = 0;
	m_iMon = 0;
	m_iDay = 0;
	m_iHour = 0;
	m_iMin = 0;
	m_iSec = 0;


	char tempDate[16] = {0};
//	AllTrim(sDateStr);
	strcpy (tempDate, sDateStr);
//	m_iSec = atoi (tempDate+12);
//	tempDate[12] = 0;
//	m_iMin = atoi (tempDate+10);
//	tempDate[10] = 0;
//	m_iHour = atoi (tempDate+8);
	tempDate[8] = 0;
	m_iDay = atoi (tempDate+6);
	tempDate[6] = 0;
	m_iMon = atoi (tempDate+4);
	tempDate[4] = 0;
	m_iYear = atoi (tempDate) - 1600;
}

void Date::stdToInnerF()
{
    if (m_iMon < 1) {
        int i = (0-m_iMon) / 12 + 1;
        m_iYear -= i;
        m_iMon += i*12;
    }
    m_iYear += (m_iMon-1) / 12;

    m_iMon = (m_iMon % 12);

    if (!m_iMon) m_iMon = 12;

    int i = m_iYear % 400;

    int n = m_iDaysSumOfOneYear[(int)(m_iYearsDays[i])][m_iMon] + m_iDay - 1;
    if(n>=m_iDaysSumOfOneYear[(int)(m_iYearsDays[i])][m_iMon+1])
        n = m_iDaysSumOfOneYear[(int)(m_iYearsDays[i])][m_iMon+1]-1;
    m_iDays = m_iYearsSumDays[i] + n;
    m_iDays += (m_iYear/400)*146097;
}
//##ModelId=423E390A01E7
void Date::stdToInner()
{
    if (m_iMon < 1) {
        int i = (0-m_iMon) / 12 + 1;
        m_iYear -= i;
        m_iMon += i*12;
    }

    m_iYear += (m_iMon-1) / 12;

    m_iMon = (m_iMon % 12);

    if (!m_iMon) m_iMon = 12;

	int i = m_iYear % 400;

	m_iDays = m_iYearsSumDays[i] + m_iDaysSumOfOneYear[(int)(m_iYearsDays[i])][m_iMon] + m_iDay - 1;
	m_iDays += (m_iYear/400)*146097;
}

void Date::stdToInnerA()
{
	int i;
	m_iSeconds = m_iHour*3600 + m_iMin*60 + m_iSec;
	if (m_iSeconds > 86399) {
		addDay (m_iSeconds/86400);
		m_iSeconds %= 86400;
	} else if (m_iSeconds < 0) {
		i = (m_iSeconds+1)/86400 - 1;	
		addDay (i);
		m_iSeconds += (0-i)*86400;
	}
}

void Date::innerToStdA()
{
	m_iHour = m_iSeconds / 3600;
	m_iMin = (m_iSeconds%3600) / 60;
	m_iSec = m_iSeconds - m_iHour*3600 - m_iMin*60;
}

//##ModelId=423E3925020E
void Date::innerToStd()
{
	int i, j;
	short k;
	
	i = m_iDays / 146097 * 400;
	//##找到年
	j = m_iDays % 146097;
	m_iYear = j / 366 + 1;
	while (j >= m_iYearsSumDays[m_iYear])
		m_iYear += 1;
	m_iYear -= 1;	
	//##最后别忘了做:m_iYear += i;
	//##找到月
	j = j - m_iYearsSumDays[m_iYear];
	m_iMon = j / 32 + 1;
	k = m_iYearsDays[m_iYear];
	while (j >= m_iDaysSumOfOneYear[k][m_iMon])
		m_iMon += 1;
	m_iMon -= 1;
		
	//##找到日
	m_iDay = j - m_iDaysSumOfOneYear[k][m_iMon] + 1;
	//##扫尾工作
	m_iYear += i;
}


//add by yangch 20070402
char *Date::getTimeString(char *sDestination, char const *sFormat)
{
	if (!sFormat) {
		sprintf (sDestination, "%04d%02d%02d%02d%02d%02d",
			this->m_iYear+1600, this->m_iMon, this->m_iDay,
			this->m_iHour, this->m_iMin, this->m_iSec);
	} else {
		char format[32];
		int i;
		char *p;
		char temp[5];

		memset (format, 0, sizeof(format));
		strncpy (format, sFormat, sizeof(format));

		//##小写变大写
		i = strlen(format);
		while (i) {
			i--;
			format[i] |= 0X20;
		}
	
		p = strstr (format, "yyyy");
		if (p) {
			sprintf(temp, "%04d", this->m_iYear+1600);
			memcpy (sDestination + (p - format), temp, 4);
		}

		p = strstr (format, "mm");
		if (p) {
			sprintf(temp, "%02d", this->m_iMon);
			memcpy (sDestination + (p - format), temp, 2);
		}

		p = strstr (format, "dd");
		if (p) {
			sprintf(temp, "%02d", this->m_iDay);
			memcpy (sDestination + (p - format), temp, 2);
		}

		p = strstr (format, "hh");
		if (p) {
			sprintf(temp, "%02d", this->m_iHour);
			memcpy (sDestination + (p - format), temp, 2);
		}

		p = strstr (format, "mi");
		if (p) {
			sprintf(temp, "%02d", this->m_iMin);
			memcpy (sDestination + (p - format), temp, 2);
		}

		p = strstr (format, "ss");
		if (p) {
			sprintf(temp, "%02d", this->m_iSec);
			memcpy (sDestination + (p - format), temp, 2);
		}
	}

	return sDestination;
}
int Date::checkDate(const char* pchString)
{
    int num;
    char tmp[10];
    int iYear,iMonth,i;

    if(strlen(pchString) != 8)
        return -1;
    int iLen = strlen(pchString);
    for (i = 0; i < iLen; i++)
    {
        if ((pchString[i] < '0') || (pchString[i] > '9'))
            return -1;
    }

    strncpy(tmp , pchString , 4);
    tmp[4] = 0;
    num = atoi(tmp);
    iYear = num;
    if((num < 1900) || (num > 3000))
        return -1;
    strncpy(tmp , pchString + 4 , 2);
    tmp[2] = 0;
    num = atoi(tmp);
    iMonth = num;
    if((num < 1) || (num > 12))
        return -1;
    strncpy(tmp , pchString + 6 , 2);
    tmp[2] = 0;
    num = atoi(tmp);
    if((num < 1) || (num > 31))
        return -1;
    switch(iMonth)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        if((num < 1) || (num > 31))
            return -1;
        break;
    case 2:
        if((iYear%4==0&&iYear%100!=0)||iYear%400==0) //judge leap year
        {
            if((num < 1) || (num > 29))
                return -1;
        }
        else
        {
            if((num < 1) || (num > 28))
                return -1;
        }
        break;
    default :
        if((num < 1) || (num > 30))
            return -1;
    }
    return 1;
}




int Date::checkTime(const char* pchString)
{
    int num,i;
    char tmp[10];

    if(strlen(pchString) != 6)
        return -1;
    int iLen = strlen(pchString);
    for (i=0;i<iLen;i++)
    {
        if ((pchString[i]<'0')||(pchString[i]>'9'))
            return -1;
    }
    strncpy(tmp , pchString , 2);
    tmp[2] = 0;
    num = atoi(tmp);
    if((num < 0) || (num > 23))
        return -1;
    strncpy(tmp , pchString + 2 , 2);
    tmp[2] = 0;
    num = atoi(tmp);
    if((num < 0) || (num > 59))
        return -1;
    strncpy(tmp , pchString + 4 , 2);
    tmp[2] = 0;
    num = atoi(tmp);
    if((num < 0) || (num > 59))
        return -1;

    return 1;
}

