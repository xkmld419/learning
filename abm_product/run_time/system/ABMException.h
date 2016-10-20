#ifndef _ABM_EXCEPTION_H_
#define _ABM_EXCEPTION_H_

#include <vector>
#include <stdio.h>

using namespace std;

struct ExceptMsg {
    char m_sFileName[32];       //发生错误的文件名
    int  m_iLineNo;            //发生错误的地方
    char m_sMsg[256];            //错误的消息内容
};	

#define ADD_EXCEPT0(X,Y) (X).addErr(__FILE__, __LINE__,Y)
#define ADD_EXCEPT1(X,Y,Z) (X).addErr(__FILE__, __LINE__,Y,Z)
#define ADD_EXCEPT2(X,Y,Z,Z1) (X).addErr(__FILE__, __LINE__,Y,Z,Z1)
#define ADD_EXCEPT3(X,Y,Z,Z1,Z2) (X).addErr(__FILE__, __LINE__,Y,Z,Z1,Z2)
#define ADD_EXCEPT4(X,Y,Z,Z1,Z2,Z3) (X).addErr(__FILE__, __LINE__,Y,Z,Z1,Z2,Z3)
#define ADD_EXCEPT5(X,Y,Z,Z1,Z2,Z3,Z4) (X).addErr(__FILE__, __LINE__,Y,Z,Z1,Z2,Z3,Z4)
#define ADD_EXCEPT6(X,Y,Z,Z1,Z2,Z3,Z4,Z5) (X).addErr(__FILE__, __LINE__,Y,Z,Z1,Z2,Z3,Z4,Z5)
#define ADD_EXCEPT7(X,Y,Z,Z1,Z2,Z3,Z4,Z5,Z6) (X).addErr(__FILE__, __LINE__,Y,Z,Z1,Z2,Z3,Z4,Z5,Z6)
#define ADD_EXCEPT8(X,Y,Z,Z1,Z2,Z3,Z4,Z5,Z6,Z7) (X).addErr(__FILE__, __LINE__,Y,Z,Z1,Z2,Z3,Z4,Z5,Z6,Z7)
#define ADD_EXCEPT12(X,Y,Z,Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8,Z9,Z10,Z11) (X).addErr(__FILE__, __LINE__,Y,Z,Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8,Z9,Z10,Z11)
	
class ABMException {
public:
    ABMException() {
        m_oErrBuf.resize (16);
        m_oErrBuf.clear ();
    }

    //当消息通过getErrs完成后，由应用程序显式地进行清空
    void clear() {
        m_oErrBuf.clear ();
    }

    //通过此方法获取到错误；该指针返回内部区域，不要对指针做删除操作
    vector<ExceptMsg> const * getErrs() {
        return &m_oErrBuf;
    }

    //通过以下方法，将错误输出到标准输出或者文件中
    //sLineHead: 错误信息的每行行首，可以传空
    void printErr(char *sLineHead=0, FILE *pFile = 0);

    //通过addErr，将错误保存起来；通常使用 ADD_EXCEPT宏
    void addErr(char const *sFile, int const iLine, char const *sFormat, ...);

private:
    vector<ExceptMsg> m_oErrBuf;
};


#endif

