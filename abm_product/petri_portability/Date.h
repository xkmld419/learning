/*VER: 1*/ 
// Copyright (c) 2005,联创科技股份有限公司电信事业部
// All rights reserved.

#ifndef DATE_H_HEADER_INCLUDED_BE09DCD0
#define DATE_H_HEADER_INCLUDED_BE09DCD0
class DateDiff
{
	public:
		int days; //相差的天数
		int secs; //相差的秒数
		int milsecs;//相差的毫秒数

		DateDiff (int iDays, int iSecs, int iMilSecs)
		: days (iDays), secs(iSecs), milsecs(iMilSecs) {}

		DateDiff ()
		: days(0), secs(0), milsecs(0) {}

		DateDiff (const DateDiff & right)
		{
			days = right.days;
			secs = right.secs;
			milsecs = right.milsecs;
		}

};

//##ModelId=41DB832001B8
class Date
{
public:
    //##ModelId=41DCA1BB012A
    Date();

    //##ModelId=41E76D9A00B9
    Date(char const *sDateStr, char const *sFormat = 0);

    //Date(const Date& dt);

    int operator- (const Date& right);

    long diffSec (Date & right);

    int diffMon (Date & right);

    int diffStrictYear (Date & right);
    int diffStrictMon (Date & right);
    int diffStrictDay (Date & right);

    //##ModelId=41E76D8503C6
    void getCurDate();

    //##ModelId=41DCA1BB013E
    virtual ~Date();

    //##ModelId=41DCA2210267
    bool operator==(const Date& right);

    //##ModelId=41DCA22102B7
    bool operator!=(const Date& right);

    //##ModelId=41DCA2210312
    bool operator<(const Date& right);

    //##ModelId=41DCA221036C
    bool operator<=(const Date& right);

    //##ModelId=41DCA22103A8
    bool operator>(const Date& right);

    //##ModelId=41DCA22103DA
    bool operator>=(const Date& right);

    //##ModelId=41DCA6920118
    int getDay() {
        return m_iDay;
    }

    //##ModelId=41DCA6920154
    void setDay(int iDay);

    //##ModelId=41DCA69201EA
    int getHour() {
        return m_iHour;
    }

    //##ModelId=41DCA6920226
    void setHour(int iHour);

    //##ModelId=41DCA692026C
    int getMilSec();

    //##ModelId=41DCA6920294
    void setMilSec(int iMilSec);

    //##ModelId=41DCA69202DA
    int getMin() {
        return m_iMin;
    }

    //##ModelId=41DCA6920303
    void setMin(int iMin);

    //##ModelId=41DCA6920353
    int getMonth() {
        return m_iMon;
    }

    //##ModelId=41DCA692037B
    void setMonth(int iMonth);

    //##ModelId=41DCA69203D5
    int getSec();

    //##ModelId=41DCA6930015
    void setSec(int iSec);

    //##ModelId=41DCA6930079
    int getWeek();

    //##ModelId=41DCA6930105
    int getYear() {
        return m_iYear + 1600;
    }

    //##ModelId=41DCA6930141
    void setYear(int iYear);

    //##ModelId=41DCA714036E
    char *toString(char * sFormat = 0);

    operator char * ();
    //##ModelId=41DCA71C02F7
    void addDay(int iDay);

    //##ModelId=41DCA7360222
    void addYear(int iYear);
    void addYearAsOracle(int iYear);

    //##ModelId=41DCA73D02E0
    void addSec(int iSec);

    //##ModelId=41DCA743004A
    void addMilSec(int iMidSec);

    //##ModelId=41DCA7490324
    void addMin(int iMin);

    //##ModelId=41DCA77F015F
    void addMonth(int iMonth);
    void addMonthAsOracle(int iMonth);

    //##ModelId=41DCA78400DA
    void addHour(int iHour);

    //##ModelId=41DCA7A10013
    int getDayNumInMonth(int iMonth);

    //##ModelId=41DCA7BB01B5
    bool isLeapYear(int iYear = 0);

    //##ModelId=41DCA7CE00EA
    bool parse(char const *sDateStr, char const *sFormat = 0);

    //##add by yangch 20070403,日期类型转化到天
    void parseToDay(char const *sDateStr);

    //## add by yangch 20070402
    char *getTimeString(char *sDestination, char const *sFormat = 0);

public:
    static int checkDate(const char *pchString);

    static int checkTime(const char *pchString) ;
public:
    static char m_sDateMin[16][9];


  private:
    //##ModelId=423E390A01E7
    inline void stdToInner();
    inline void stdToInnerF();

    //##ModelId=423E3925020E
    //##ModelId=423E3925020E
    inline void innerToStd();

    inline void stdToInnerA();
    inline void innerToStdA();

    //##ModelId=41DCA229002E
    int m_iYear;

    //##ModelId=41DCA23503B1
    int m_iMon;

    //##ModelId=41DCA23B014C
    int m_iDay;

    //##ModelId=41DCA23E0359
    int m_iHour;

    //##ModelId=41DCA2460166
    int m_iMin;

    //##ModelId=41DCA29F01C8
    int m_iSec;

    //##ModelId=41DCA2DF0116
    int m_iWeek;

    //##ModelId=41DCA592032C
    int m_iMilSec;
    //##ModelId=423E339900B8
    //##Documentation
    //## 每年的天数，（12个月，分闰年和非闰年）
    static short m_iDaysSumOfOneYear[2][14];
    static short m_iDaysOfOneYear[2][13];

    //##ModelId=423E33EB03AF
    //##Documentation
    //## 每年的天数是400年一个周期，从1600年开始
    static int m_iYearsSumDays[401];

    //##ModelId=423E3D3D02C9
    static char m_iYearsDays[400];
    
    //##ModelId=423E372C001D
    //##Documentation
    //## 日期的内部存放分为两个部分，一个为距离1600年1月1日的天数，一个为当天经过的秒数。m_iDays存放的是天数，m_iSeconds存放的是秒数
    int m_iDays;

    //##ModelId=423E373503D8
    int m_iSeconds;

    char m_sOutBuf[32];
};



#endif /* DATE_H_HEADER_INCLUDED_BE09DCD0 */
