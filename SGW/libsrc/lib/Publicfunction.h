#ifndef _PUBLIC_FUNCTION_H_
#define _PUBLIC_FUNCTION_H_

/*读取配置文件中的数据库连接信息*/
int getConnectDBinfo(char * aimstr, char *username, char * password, char * connstr, char * cachepwd);

/*去掉字符串左右的空格*/
//void Trim(char * String);

/*将字符串全部改为大写*/
//void MakeUpper(char * pchString);

/*将字符串全部改为小写*/
//void MakeLower(char * pchString);

void disp_buf(char *buf, int size);

#endif



