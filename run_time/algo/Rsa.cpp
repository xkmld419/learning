#include "Rsa.h" 
#include <stdio.h> 

//构造大数对象并初始化为零 
CRsa::CRsa() 
{ 
    m_nLength=1; 
    for(int i=0;i <BI_MAXLEN;i++)m_ulValue[i]=0; 
} 

//大数比较 
int CRsa::Cmp(CRsa A) 
{ 
    if(m_nLength>A.m_nLength)return 1; 
    if(m_nLength <A.m_nLength)return -1; 
    for(int i=m_nLength-1;i>=0;i--) 
    { 
        if(m_ulValue[i]>A.m_ulValue[i])return 1; 
        if(m_ulValue[i] <A.m_ulValue[i])return -1; 
    } 
    return 0; 
} 

//大数赋值 
void CRsa::Mov(CRsa A) 
{ 
    m_nLength=A.m_nLength; 
    for(int i=0;i <BI_MAXLEN;i++) 
    m_ulValue[i]=A.m_ulValue[i]; 
} 
void CRsa::Mov(unsigned __int64 A) 
{ 
    if(A>0xffffffff) 
    { 
        m_nLength=2; 
        m_ulValue[1]=(unsigned int)(A>>32); 
        m_ulValue[0]=(unsigned int)A; 
    } 
    else 
    { 
        m_nLength=1; 
        m_ulValue[0]=(unsigned int)A; 
    } 
    for(int i=m_nLength;i <BI_MAXLEN;i++)m_ulValue[i]=0; 
} 

//大数相减 
CRsa CRsa::SubCRsa(CRsa A) 
{ 
    CRsa X; 
    X.Mov(*this); 
    if(X.Cmp(A) <=0){X.Mov(0);return X;} 
    unsigned carry=0; 
    unsigned __int64 num; 
    unsigned i; 
    for(i=0;i <m_nLength;i++) 
    { 
        if((m_ulValue[i]>A.m_ulValue[i])||((m_ulValue[i]==A.m_ulValue[i])&&(carry==0))) 
        { 
            X.m_ulValue[i]=m_ulValue[i]-carry-A.m_ulValue[i]; 
            carry=0; 
        } 
        else 
        { 
            num=0x100000000+m_ulValue[i]; 
            X.m_ulValue[i]=(unsigned int)(num-carry-A.m_ulValue[i]); 
            carry=1; 
        } 
    } 
    while(X.m_ulValue[X.m_nLength-1]==0)X.m_nLength--; 
    return X; 
} 


CRsa CRsa::Sub(unsigned int A) 
{ 
    CRsa X; 
    X.Mov(*this); 
    if(X.m_ulValue[0]>=A){X.m_ulValue[0]-=A;return X;} 
    if(X.m_nLength==1){X.Mov(0);return X;} 
    unsigned __int64 num=0x100000000+X.m_ulValue[0]; 
    X.m_ulValue[0]=(unsigned int)(num-A); 
    int i=1; 
    while(X.m_ulValue[i]==0){X.m_ulValue[i]=0xffffffff;i++;} 
    X.m_ulValue[i]--; 
    if(X.m_ulValue[i]==0)X.m_nLength--; 
    return X; 
} 

//大数相乘 
CRsa CRsa::MulCRsa(CRsa A) 
{ 
    if(A.m_nLength==1)return Mul(A.m_ulValue[0]); 
    CRsa X; 
    unsigned __int64 sum,mul=0,carry=0; 
    unsigned i,j; 
    X.m_nLength=m_nLength+A.m_nLength-1; 
    for(i=0;i <X.m_nLength;i++) 
    { 
        sum=carry; 
        carry=0; 
        for(j=0;j <A.m_nLength;j++) 
        { 
            if(((i-j)>=0)&&((i-j) <m_nLength)) 
            { 
                mul=m_ulValue[i-j]; 
                mul*=A.m_ulValue[j]; 
                carry+=mul>>32; 
                mul=mul&0xffffffff; 
                sum+=mul; 
            } 
        } 
        carry+=sum>>32; 
        X.m_ulValue[i]=(unsigned int)sum; 
} 
    if(carry){X.m_nLength++;X.m_ulValue[X.m_nLength-1]=(unsigned int)carry;} 
    return X; 
} 


CRsa CRsa::Mul(unsigned int A) 
{ 
    CRsa X; 
    unsigned __int64 mul; 
    unsigned int carry=0; 
    X.Mov(*this); 
    for(unsigned i=0;i <m_nLength;i++) 
    { 
        mul=m_ulValue[i]; 
        mul=mul*A+carry; 
        X.m_ulValue[i]=(unsigned int)mul; 
        carry=(unsigned int)(mul>>32); 
    } 
    if(carry){X.m_nLength++;X.m_ulValue[X.m_nLength-1]=carry;} 
    return X; 
} 

//大数相除 
CRsa CRsa::Div(unsigned int A) 
{ 
    CRsa X; 
    X.Mov(*this); 
    if(X.m_nLength==1){X.m_ulValue[0]=X.m_ulValue[0]/A;return X;} 
    unsigned __int64 div,mul; 
    unsigned int carry=0; 
    for(int i=X.m_nLength-1;i>=0;i--) 
    { 
        div=carry; 
        div=(div << 32)+X.m_ulValue[i]; 
        X.m_ulValue[i]=(unsigned int)(div/A); 
        mul=(div/A)*A; 
        carry=(unsigned int)(div-mul); 
    } 
    if(X.m_ulValue[X.m_nLength-1]==0)X.m_nLength--; 
    return X; 
} 


//大数求模 
CRsa CRsa::ModCRsa(CRsa A) 
{ 
  if(A.m_nLength==1) 
  { 
      CRsa X; 
      X.m_ulValue[0]=Mod(A.m_ulValue[0]); 
      return X; 
  } 
  CRsa X,Y; 
  int len; 
  unsigned __int64 num,div; 
  unsigned int carry=0; 
  X.Mov(*this); 
  while(X.Cmp(A)>0) 
  {      
      if(X.m_ulValue[X.m_nLength-1]>A.m_ulValue[A.m_nLength-1]) 
      { 
          len=X.m_nLength-A.m_nLength; 
          div=X.m_ulValue[X.m_nLength-1]/(A.m_ulValue[A.m_nLength-1]+1); 
      } 
      else if(X.m_nLength>A.m_nLength) 
      { 
          len=X.m_nLength-A.m_nLength-1; 
          num=X.m_ulValue[X.m_nLength-1]; 
          num=(num << 32)+X.m_ulValue[X.m_nLength-2]; 
          if(A.m_ulValue[A.m_nLength-1]==0xffffffff)div=(num>>32); 
          else div=num/(A.m_ulValue[A.m_nLength-1]+1); 
      } 
      else 
      { 
          X.Mov(X.SubCRsa(A)); 
          break; 
      } 
      Y.Mov(div); 
      Y.Mov(Y.MulCRsa(A)); 
      Y.m_nLength+=len; 
      for(int i=Y.m_nLength-1;i>=len;i--) Y.m_ulValue[i]=Y.m_ulValue[i-len]; 
      for(int i=0;i <len;i++)  Y.m_ulValue[i]=0; 
      X.Mov(X.SubCRsa(Y)); 
  } 
  if(X.Cmp(A)==0)X.Mov(0); 
  return X; 
} 

unsigned int CRsa::Mod(unsigned int A) 
{ 
  if(m_nLength==1)return(m_ulValue[0]%A); 
  unsigned __int64 div; 
  unsigned int carry=0; 
  for(int i=m_nLength-1;i>=0;i--) 
  { 
      div=carry*0x100000000+m_ulValue[i]; 
      carry=(unsigned int)(div-((div/A)*A)); 
  } 
  return carry; 
} 


//蒙哥马利算法求模幂 
CRsa CRsa::ModExp(CRsa A, CRsa B) 
{ 
  CRsa X,Y,Z; 
  X.Mov(1); 
  Y.Mov(*this); 
  Z.Mov(A); 
  while((Z.m_nLength!=1)||Z.m_ulValue[0]) 
  { 
      if(Z.m_ulValue[0]&1) 
      { 
          Z.Mov(Z.Sub(1)); 
          X.Mov(X.MulCRsa(Y)); 
          X.Mov(X.ModCRsa(B)); 
      } 
      else 
      { 
          Z.Mov(Z.Div(2)); 
          Y.Mov(Y.MulCRsa(Y)); 
          Y.Mov(Y.ModCRsa(B)); 
      } 
  } 
  return X; 
} 

//加密
int CRsa::encrypted(char A,unsigned int s[])
{
	int nResult = -1;
	try
	{
		CRsa  m,e,n,c; 
		int i; 

		// 需要加解密的信息
		m.m_nLength=4; 
		m.m_ulValue[3]=0x0; 
		m.m_ulValue[2]=0x0; 
		m.m_ulValue[1]=0x0; 
		//   m.m_ulValue[0]=0x45e; 
		m.m_ulValue[0]=A; 
/*
		printf("待加密的原文: "); 
		for(i=BI_MAXLEN-5;i>=0;i--) 
		//      printf("%x\n",m.m_ulValue[i]); 
		printf("%c",m.m_ulValue[BI_MAXLEN-5-i]); 
		printf("\n"); 
*/

		// 公钥指数， 一般为3、17、65537，也可以为是其它一个素数，
		e.m_nLength=1; 
		e.m_ulValue[0]=0x34703; 

		// 模数n, 一个大素数
		n.m_nLength=4; 
		n.m_ulValue[3]=0x78CDED41; 
		n.m_ulValue[2]=0xF71A0BF9; 
		n.m_ulValue[1]=0xA1F36E0A; 
		n.m_ulValue[0]=0x60F3EE89; 

		// 加密过程
		// c  = (m^e) % n;
		// c就是密文
		c.Mov(m.ModExp(e,n)); 

		printf("加密后的密文: "); 
		for(i=BI_MAXLEN-5;i>=0;i--) 
		//        printf("%x\n",c.m_ulValue[i]); 
		printf("%08x",c.m_ulValue[BI_MAXLEN-5-i]); 
		printf("\n"); 

		s[0]=c.m_ulValue[0];
		s[1]=c.m_ulValue[1];
		s[2]=c.m_ulValue[2];
		s[3]=c.m_ulValue[3];
/*
		  printf("begin\n");
		  printf("%d\n",c.m_nLength);
		  for(int m=0;m<8;m++)
		  printf("%x",c.m_ulValue[m]);
		  printf("end\n");
*/
		nResult = 0;
	}
	catch(...)
	{
	
	}
	return nResult;
}

//解密
int CRsa::decrypted(unsigned int s[],char &A)
{
	int nResult = -1;
	try
	{
		CRsa  c,n,d,m1;
		int i;
		c.m_nLength=4;
		c.m_ulValue[0]=s[0];
		c.m_ulValue[1]=s[1];
		c.m_ulValue[2]=s[2];
		c.m_ulValue[3]=s[3];
/*
				  printf("begin\n");
		  printf("%d\n",c.m_nLength);
		  for(int m=0;m<8;m++)
		  printf("%x",c.m_ulValue[m]);
		  printf("end\n");
*/
		// 模数n, 一个大素数
		n.m_nLength=4; 
		n.m_ulValue[3]=0x78CDED41; 
		n.m_ulValue[2]=0xF71A0BF9; 
		n.m_ulValue[1]=0xA1F36E0A; 
		n.m_ulValue[0]=0x60F3EE89; 

		// 私钥指数， 与e对应
		d.m_nLength=4; 
		d.m_ulValue[3]=0x4C9327F3; 
		d.m_ulValue[2]=0x70B028FD; 
		d.m_ulValue[1]=0x08D56387; 
		d.m_ulValue[0]=0xFCC6161B; 

		// 解密操作
		// m1 = (c^d) % n
		// m1 就是得到的明文, 它的值应当等于 m.
		m1.Mov(c.ModExp(d,n)); 
/*
		printf("解密后的明文: "); 
		for(i=BI_MAXLEN-5;i>=0;i--) 
		//   printf("%x\n",m1.m_ulValue[i]); 
		printf("%c",m1.m_ulValue[BI_MAXLEN-5-i]); 
		printf("\n"); 
*/
		A = m1.m_ulValue[0];
		nResult = 0;
	}
	catch(...)
	{
	
	}
	return nResult;
}

////////////////////////////////////////////////////////////////////////////////

//加密
int CRsa::encrypted(char *A,vector<unsigned int> &vUL)
{
	int nResult = -1;
	int iStringLength = 0;
	int iModNum = 0;
	int iDivNum = 0;
	int iCycleNum = 0;
	try
	{
		if( (A == NULL) || ( (iStringLength=strlen(A))<1 ) )//需要加密的字符串为空
		{
			return 1;
		}
		iModNum = iStringLength%4;
		if( 0 == iModNum )
			iDivNum = iStringLength/4;
		else
			iDivNum = iStringLength/4+1;

	  for(int mm=0;mm<iDivNum;mm++)
	  {
		CRsa  m,e,n,c; 
		int i=0,j=0;

		// 需要加解密的信息
		m.m_nLength=4; 
		if(mm < iDivNum-1)//非待加密的最后一组子字符串
		{
			for(j=0;j<4;j++)	
				m.m_ulValue[j]=A[iCycleNum++];
		}
		else
		{
			if(iModNum == 0)//字符串长度为4的整数倍
			{
				for(j=0;j<4;j++)	
					m.m_ulValue[j]=A[iCycleNum++];
			}
			else
			{
				for(j=0;j<4;j++)
				{
					if(j<iModNum)
						m.m_ulValue[j]=A[iCycleNum++];
					else
						m.m_ulValue[j]=0x0;
				}
			}
		}

		// 公钥指数， 一般为3、17、65537，也可以为是其它一个素数，
		e.m_nLength=1; 
		e.m_ulValue[0]=0x34703; 

		// 模数n, 一个大素数
		n.m_nLength=4; 
		n.m_ulValue[3]=0x78CDED41; 
		n.m_ulValue[2]=0xF71A0BF9; 
		n.m_ulValue[1]=0xA1F36E0A; 
		n.m_ulValue[0]=0x60F3EE89; 

		// 加密过程
		// c  = (m^e) % n;
		// c就是密文
		c.Mov(m.ModExp(e,n)); 

		for(j=0;j<4;j++)
			vUL.push_back(c.m_ulValue[j]);

	  }
		nResult = 0;
	}
	catch(...)
	{
	}
	return nResult;
}

//解密
int CRsa::decrypted(vector<unsigned int>vUL,char *A)
{
	std::string sString="";
	int iDivNum = 0;
	int iCycleNum = 0;
	int nResult = -1;
	try
	{
		if( vUL.size() % 4 != 0 )//必须为4的整数倍
		{
			return 1;
		}
		iDivNum = vUL.size()/4;
	  for(int mm=0;mm<iDivNum;mm++)
	  {

		CRsa  c,n,d,m1;
		int i=0,j=0;
		c.m_nLength=4;
		for(j=0;j<4;j++)	
			c.m_ulValue[j]=vUL[iCycleNum++];

		// 模数n, 一个大素数
		n.m_nLength=4; 
		n.m_ulValue[3]=0x78CDED41; 
		n.m_ulValue[2]=0xF71A0BF9; 
		n.m_ulValue[1]=0xA1F36E0A; 
		n.m_ulValue[0]=0x60F3EE89; 

		// 私钥指数， 与e对应
		d.m_nLength=4; 
		d.m_ulValue[3]=0x4C9327F3; 
		d.m_ulValue[2]=0x70B028FD; 
		d.m_ulValue[1]=0x08D56387; 
		d.m_ulValue[0]=0xFCC6161B; 

		// 解密操作
		// m1 = (c^d) % n
		// m1 就是得到的明文, 它的值应当等于 m.
		m1.Mov(c.ModExp(d,n)); 

		for(j=0;j<4;j++)
			sString += (char)m1.m_ulValue[j];
	  }
	  strcpy(A,sString.c_str());
		nResult = 0;
	}
	catch(...)
	{
	
	}
	return nResult;
}
