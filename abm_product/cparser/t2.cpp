/*VER: 1*/ 
#include <iostream>
#include <string>

struct StrList {
	char *  str;
	StrList * pNext;
};


int main(int argc, char * argv[])
{
	StrList * pHead, * pCur;
	string s1;
	
	int i;
	
	pCur = pHead = (struct StrList *)malloc(sizeof(StrList));
	pCur->pNext =NULL;
	for ( i=0;i<10000000; i++){
		
		pCur->str = strdup("abc");
		
		pCur->pNext = (struct StrList *)malloc(sizeof(StrList));
		pCur = pCur->pNext;
		pCur->pNext =NULL;
		if ( i%1000000 == 0){
			cout<<"i="<<i<<endl;
			system("date");
		}
	}
}
