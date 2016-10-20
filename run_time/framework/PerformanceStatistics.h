#include <vector>
#include <string>
#include <iostream>
#include <sys/time.h>

using namespace std;

#ifndef PERFORMANCESTATISTICS_H
#define PERFORMANCESTATISTICS_H
#define PRECISION_UNIT 1000000

struct tPerformanceStatistics
{
	long lConsumeTime;
	bool bExists;	
};

/*系统统计类(精确到微妙)*/
class PerformanceStatistics
{
	private:
		static int iMaxWatchPointNumber;
		static vector<tPerformanceStatistics> vConsumeTime;
		static timeval start_timeval, end_timeval;
	public:
		PerformanceStatistics();
		~PerformanceStatistics();
		static void init(int iWatchPointNumber);
		static void beginStatistics();
		static void endStatistics(int iWatchPoint);
		static void outputStatisticsResult();
};

#endif