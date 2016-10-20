//Í·ÎÄ¼þRsa.h 
#include <vector>
#include <string>
#include <iostream>

using namespace std;

#ifndef RSA_H 
#define RSA_H 


#define BI_MAXLEN 8 

class CRsa 
{ 
public: 
unsigned      m_nLength; 
unsigned int m_ulValue[BI_MAXLEN]; 
CRsa(); 
  
void Mov(unsigned __int64 A); 
void Mov(CRsa A); 

CRsa Sub(unsigned int A); 
CRsa SubCRsa(CRsa A); 

CRsa Mul(unsigned int A); 
CRsa MulCRsa(CRsa A); 

unsigned int Mod(unsigned int A); 
CRsa ModCRsa(CRsa A); 

CRsa Div(unsigned int A); 
int Cmp(CRsa A); 
CRsa ModExp(CRsa A, CRsa B); 

int encrypted(char A,unsigned int s[]);
int decrypted(unsigned int s[],char &A);

int encrypted(char *A,vector<unsigned int> &vUL);
int decrypted(vector<unsigned int>vUL,char *A);

}; 

#endif 
