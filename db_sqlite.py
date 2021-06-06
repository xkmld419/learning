__author__ = 'PanTianxing'
#coding=utf-8

import sqlite3
from threading import Lock

db_lock = Lock()


class SQLLite:
    def __init__(self, data_name):
        self.db = sqlite3.connect(data_name)
        self.cursor = self.db.cursor()

    def db_create(self, sql):
        try:
            db_lock.acquire()
            self.cursor.execute(sql)
            self.db.commit()
            db_lock.release()
        except Exception as err:
            self.db.commit()
            db_lock.release()
            raise err
        pass

    def insert(self, sql, data_list):
        try:
            db_lock.acquire()
            self.cursor.execute(sql, data_list)
            self.db.commit()
            db_lock.release()
        except Exception as err:
            self.db.commit()
            db_lock.release()
            raise err

    def execute(self, sql):
        try:
            db_lock.acquire()

            self.cursor.execute(sql)
            self.db.commit()

            db_lock.release()
        except Exception as err:
            self.db.commit()
            db_lock.release()
            raise err

    def bulk_insert(self, sql, data_list_list):
        try:

            db_lock.acquire()
            self.cursor.executemany(sql, data_list_list)
            self.db.commit()

            db_lock.release()
        except Exception as err:
            self.db.commit()
            db_lock.release()
            raise err

    def select(self, sql):
        try:
            db_lock.acquire()

            self.cursor.execute(sql)
            result = self.cursor.fetchall()

            db_lock.release()
            return result
        except Exception as err:
            self.db.commit()
            db_lock.release()
            raise err

    def common_insert(self, table_name, column_list, value_list):
        column_str = ''
        if len(column_list) > 0:
            column_str = ','.join(column_list)
            column_str = '('+column_str+')'

        s_holder = []

        try:
            length = len(value_list[0])
            for i in range(length):
                s_holder.append('%s')
            placeholder = ','.join(s_holder)
            sql = ' '.join(['replace into', table_name, column_str, 'values (', placeholder, ')'])
            # sql = "replace into "+table_name+' '+column_str+' '+'values ('+placeholder+')'
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

    def shutdown(self):
        self.db.close()
