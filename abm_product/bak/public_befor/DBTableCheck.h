#ifndef __DB_TABLE_CHECK_H__
#define __DB_TABLE_CHECK_H__

class DBTableCheck
{
public:
	/*
        功能：检查指定表的指定列信息是否存在（属主可选）
		返回值类型：bool
		返回值含义：
			false 不存在
			true 存在
    */
	static bool checkTabColumnExist(char * sTabName, char * sColumnName,char * sOwner = NULL);
};

#endif //__DB_TABLE_CHECK_H__
