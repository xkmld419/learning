/*VER: 1*/ 
#include <iostream>
int main()
{
   int j;
   int & i=j;
   i = 0;
   j =10;
   cout<<"i="<<i<<endl;
}
 
