#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int m_iYearsDays[400] = 
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

class A;
class B;

typedef int (*FN)(A* a);

int ff(FN fn, A* a);

int pp(A* a);

int k = 0;
class A {
public:
    A() {
        x = NULL;
        y = new char[2];
        memcpy(y, "2", 2);
        f = k++;
    }
    virtual ~A() {
        if (x) delete x;
        if (y) delete y;
        x = NULL;
    }

    virtual int p() {
        printf("A is %s\n", y);
        return 0;
    }
    
    int flush() {
        return ff(p, this);
    }
public:
    char * x;
    char * y;
    int    f;
    
};

class B : public A {
public:
    B() {
        x = new char[2];
        memcpy(x, "1", 2);
        f = k++;
    }
    virtual ~B() {
        if (x) delete x;
        x = NULL;
    }
public:
    /*
    virtual int p() {
        printf("B is %s\n", x);
        return 0;
    }
    */
    virtual int p(int z) {
        printf("B is %s\n", x);
        return 0;
    }
};

int ff(FN fn, A* a)
{
    return fn(a);
}

int pp(A* a)
{
        a->p();
        return 0;
}

int main(int argc, char* argv[])
{
    char x  = '0';
    printf("x is %d \n", int(x) - 48);
    return 0;
    B *a;
    a = new B();
    delete a;
    return 0;

    char  xx[10];
    char  yy[10];
    memset(xx, 0x00, 10);
    memset(yy, 0x00, 10);

    xx[0] = 0x02;
    xx[1] = 0x03;
    xx[2] = 0x00;
    xx[3] = 0x04;
    xx[4] = 0x00;
    xx[5] = 0x04;

    strcpy(yy, xx);

    for (int i = 0; i <10; i++) {
        printf("xx[%d] is %02x, yy[%d] is %02x \n", i, xx[i], i, yy[i]);
    }

    return 0;
}
