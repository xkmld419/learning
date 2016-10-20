#ifndef __NEWVECTOR_H__
#define __NEWVECTOR_H__

//vector释放的阀值
#define FREE_SIZE 80000

#include <vector>
using  namespace std;

template <class T>
class newVector : public vector<T>
{
	public:
		newVector()
		{
			lCurrentStartPosition = 0;
			lSize = 0;
			lSumElementCount = 0;
		}

		void clear()
		{
			lCurrentStartPosition = 0;
			lSize = 0;
			lSumElementCount = 0;
			vector<T>::clear();
		}

		void push_back(const T &X)
		{
			vector<T>::push_back(X);
			lCurrentStartPosition++;
			lSize++;
			lSumElementCount++;
		}

		bool getElement(T &X)
		{
			if( lCurrentStartPosition < lSumElementCount )//有元素可用
			{
				lCurrentStartPosition++;
				lSize++;
				X = (*this)[lCurrentStartPosition-1];
				return true;
			}
			else                                          //没有元素可用
			{
				X = NULL;
				return false;
			}
		}

    void reset()//便于从头开始使用元素
    {
			lCurrentStartPosition = 0;
			lSize = 0;
			if( lSumElementCount > FREE_SIZE )
			{
				for(int i=0;i<lSumElementCount;i++)
				{
					delete (*this)[i];
				}
				clear();
			}
		}
    void focusReset()//for select use zhu
    {
				lCurrentStartPosition = 0;
				lSize = 0;
				for(int i=0;i<lSumElementCount;i++)
				{
					delete (*this)[i];
				}
				clear();
	}

		long getSumElementCount()//得到vector中存放的元素个数，为释放整个vector所用
		{
			return lSumElementCount;		
		}

		long size()
		{
			return lSize;
		}

	private:
		long lCurrentStartPosition;  //本次开始的下标
		long lSize;
		long lSumElementCount;       //vector中存放的元素个数
		
};

#endif
