#ifndef __STRCASECMP_H__
#define __STRCASECMP_H__
#if defined(_WIN32)
bool inline strcasecmp(const char *ori, const char *des)
{
 int j,nLen1,nLen2;
 bool sameChar;
 nLen1 = strlen(ori);
 nLen2 = strlen(des);
 if (nLen1!=nLen2) return false;
 sameChar = true;
 for (j=0; j<nLen1; j++)
  sameChar = sameChar && ( toupper(ori[j]) == toupper(des[j]) );
 return sameChar;
}
#endif
#endif




