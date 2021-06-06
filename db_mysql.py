# coding:utf-8
import pymysql
from threading import Lock

db_lock = Lock()

#数据库连接基类
class db_mysql:
    #初始化
    def __init__(self, host, port,user, passwd, database, charset='utf8'):
        self.host = host
        self.port = port
        self.user = user
        self.passwd = passwd
        self.database = database
        self.charset = charset
        try:
            self.db = pymysql.connect(host=self.host,
                                      user=self.user,
                                      passwd=self.passwd,
                                      db=self.database,
                                      port=self.port,
                                      charset=self.charset,
                                      use_unicode=True)
            self.cursor = self.db.cursor()
            #基本设置
            self.cursor.execute('SET NAMES utf8;')
            self.cursor.execute('SET CHARACTER SET utf8;')
            self.cursor.execute('SET character_set_connection=utf8;')
            self.db.commit()

            self.cursor.execute("SELECT VERSION()")

            ver = self.cursor.fetchone()

            print ("Database version : %s " % ver)
            
        except Exception as err:
            print (err)
            raise err

    def test(self):
        try:
            db_lock.acquire()
            self.cursor.execute("select sysdate()")
            self.db.commit()
            db_lock.release()
        except  Exception as err:
            db_lock.release()
            self.db = pymysql.connect(host=self.host,
                                      user=self.user,
                                      passwd=self.passwd,
                                      db=self.database,
                                      port=self.port,
                                      charset=self.charset,
                                      use_unicode=True)
            self.cursor = self.db.cursor()
            #基本设置
            self.cursor.execute('SET NAMES utf8;')
            self.cursor.execute('SET CHARACTER SET utf8;')
            self.cursor.execute('SET character_set_connection=utf8;')
            self.db.commit()

    #批量执行
    def executeTrans(self,sqlList):
        try:
            self.test()
            db_lock.acquire()
            for sql in sqlList:
                self.cursor.execute(sql)
            self.db.commit()

            db_lock.release()
        except Exception as err:
            self.db.commit()
            db_lock.release()
            raise err

    #执行  
    def execute(self, sql):
        try:
            self.test()
            db_lock.acquire()
       
            self.cursor.execute(sql)
            self.db.commit()
       
            db_lock.release()
        except Exception as err: 
            self.db.commit()       
            db_lock.release() 
            raise err   
   
    #单个插入
    def insert(self, sql, data_list):
        try:
            self.test()
            db_lock.acquire()
            self.cursor.execute(sql, data_list)
            self.db.commit()
            db_lock.release()
        except Exception as err:  
            self.db.commit()
            db_lock.release()      
            raise err 
     
    #批量插入，大部分都用这个插入
    def bulk_insert(self, sql, data_list_list):
        try:
            self.test()
            db_lock.acquire()
            self.cursor.executemany(sql, data_list_list)
            self.db.commit()

            db_lock.release()
        except Exception as err:
            self.db.commit()
            db_lock.release()
            raise err

    #查询
    def select(self, sql):
        try:
            self.test()
            db_lock.acquire()
    
            self.cursor.execute(sql)
            result = self.cursor.fetchall()
            self.db.commit()
    
            db_lock.release()
            return result
        except Exception as err:  
            self.db.commit()   
            db_lock.release()       
            raise err
    # 插入操作的封装一般封装，columnlist表示表名结构，value_list表示值
    def commoninsert(self, tablename, column_list, value_list):
        column_str = ''
        if ( len(column_list)>0 ):
            column_str = ','.join(column_list)
            column_str = '('+column_str+')'
    
        #生成占位符placeholder
        #不论数据何类型，都用%s，否则报错
        s_holder = []
        
        try:
            self.test()
            length = len(value_list[0])
            for i in range(length):
                s_holder.append('%s')
            placeholder = ','.join(s_holder)
            sql = "replace into "+tablename+' '+column_str+' '+'values ('+placeholder+')'
            self.bulk_insert(sql, value_list)
        except Exception as err:
            print (err)
            
    def select_column(self, sql):
        result = self.select(sql)
        return [row[0] for row in result]

    def select_row(self, sql):
        result = self.select(sql)
        return result[0]

    def select_scalar(self, sql):
        row = self.select_row(sql)
        return row[0]

    def get_table_id(self, sql):
        cur = self.db.cursor()
        cur.execute(sql)
        row = cur.fetchall()
        return row[len(row)-1]
            
            
def main():
    pass

if __name__ == '__main__':
    main()
