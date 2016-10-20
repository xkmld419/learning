#include "PerformanceStatistics.h"

int PerformanceStatistics::iMaxWatchPointNumber=0;
vector<tPerformanceStatistics> PerformanceStatistics::vConsumeTime;
timeval PerformanceStatistics::start_timeval;
timeval PerformanceStatistics::end_timeval;

PerformanceStatistics::PerformanceStatistics()
{
	vConsumeTime.clear();
}

PerformanceStatistics::~PerformanceStatistics()
{

}

void PerformanceStatistics::init(int iWatchPointNumber)
{
	vConsumeTime.clear();
	for(int i=0;i<=iWatchPointNumber;i++)
	{
		tPerformanceStatistics cl_tPerformanceStatistics;
		memset(&cl_tPerformanceStatistics,'\0',sizeof(tPerformanceStatistics));
		vConsumeTime.push_back(cl_tPerformanceStatistics);
	}
	iMaxWatchPointNumber = iWatchPointNumber;
}

void PerformanceStatistics::beginStatistics()
{
	gettimeofday(&start_timeval, NULL);
}

void PerformanceStatistics::endStatistics(int iWatchPoint)
{
	static long lTemp = 0;
	if(iMaxWatchPointNumber < iWatchPoint)
	{
		for(int i=iMaxWatchPointNumber+1;i<=iWatchPoint;i++)
		{
			tPerformanceStatistics cl_tPerformanceStatistics;
			memset(&cl_tPerformanceStatistics,'\0',sizeof(tPerformanceStatistics));
			vConsumeTime.push_back(cl_tPerformanceStatistics);
		}
		iMaxWatchPointNumber = iWatchPoint;
	}
	gettimeofday(&end_timeval, NULL);
	lTemp = (end_timeval.tv_sec*PRECISION_UNIT+end_timeval.tv_usec)-(start_timeval.tv_sec*PRECISION_UNIT+start_timeval.tv_usec);
	vConsumeTime[iWatchPoint].lConsumeTime+=lTemp;
	vConsumeTime[iWatchPoint].bExists = true;
	vConsumeTime[0].lConsumeTime+=lTemp;
}

void PerformanceStatistics::outputStatisticsResult()
{
	for(int i=0;i<=iMaxWatchPointNumber;i++)
	{
		if(i==0)
			cout<<"总耗时："<<vConsumeTime[i].lConsumeTime<<" 微妙"<<endl;
		else if(vConsumeTime[i].bExists)
			cout<<"性能统计点["<<i<<"]耗时："<<vConsumeTime[i].lConsumeTime<<" 微妙，占总耗时的："<<(static_cast<double>(vConsumeTime[i].lConsumeTime)/static_cast<double>(vConsumeTime[0].lConsumeTime))*100<<"%"<<endl;
	}
}
