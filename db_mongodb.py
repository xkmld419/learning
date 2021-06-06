#coding=utf-8

import pymongo
from pymongo import UpdateOne,ReplaceOne


class db_mongodb(object):
    def __init__(self, host, port, database, user=None, passwd=None, slaveOk=True):
        # self.connection = pymongo.Connection(host=host, port=port, slave_okay=slaveOk)
        self.connection = pymongo.MongoClient(host=host, port=port)
        self.db = self.connection[database]
        if not (user is None or user == ""):
            # self.db = connection.tage
            self.db.authenticate(user, passwd)

    def db_insert(self, collection, dic):
        self.db[collection].insert_one(dic)
        pass

    def db_count(self, collection, dic={}):
        return self.db[collection].find(dic).count()

    def db_distinct(self, collection,sKey, dic={}):
        return self.db[collection].find(dic).distinct(sKey)

    def db_find(self, collection, dict_search):
        cursor = self.db[collection].find(dict_search)
        return cursor

    def db_find_one(self, collection, dict_search):
        dict_value = self.db[collection].find_one(dict_search)
        return dict_value

    def db_find_limit(self, collection, dict_search,limitNum,sSortKey):
        cursor = self.db[collection].find(dict_search).sort(sSortKey,pymongo.ASCENDING).limit(limitNum)  #DESCENDING
        return cursor

    def db_remove(self, collection, dic):
        self.db[collection].remove(dic)

    def db_ensure_index(self, collection, lst_tuple):
        self.db[collection].ensure_index(lst_tuple)

    def db_drop_index(self, collection, lst_tuple):
        self.db[collection].drop_index(lst_tuple)

    def db_update(self, collection, dict_key, document, multi=False):
        self.db[collection].update(dict_key, document, multi=multi)

    def db_updateI(self, collection, dict_key, document, multi=False):
        self.db[collection].update(dict_key, document, multi=multi)

    def db_updateL(self, collection, dict_key, document, upset=True,multi=True):
        self.db[collection].update(dict_key, document, multi=multi)
        # self.db[collection].bulk_write([UpdateOne({"_id":"a"},{"$set":{"n":"aa"}}, upsert=True), UpdateOne({"_id":"b"},{"$set":{"n":"b"}}, upsert=True)])

    def dbBulkWrite(self,collection,listInfo):
        pyList=[]
        # pyList.append(UpdateOne({"_id":"a"},{"$set":{"n":"aa"}}))
        # pyList.append(UpdateOne({"_id":"b"},{"$set":{"n":"bb"}}))
        for item in listInfo:
            pyList.append(UpdateOne(item['query'],item['value'],upsert=True))
        self.db[collection].bulk_write(pyList)

    def change_database(self, database):
        """
        切换数据库
        """
        self.db = self.connection[database]
        
    def db_find_sort(self,collection,dict_search,field,sortflag='asc'):
        if sortflag=='desc':
            cursor = self.db[collection].find(dict_search).sort(field,pymongo.ASCENDING)
        cursor = self.db[collection].find(dict_search).sort(field,pymongo.DESCENDING)
        return cursor
    def db_rename_column(self,collection,sourcename,currentname):
        self.db[collection].update({},{"$rename":{sourcename:currentname}},False,True);
    
    def db_group_by(self,collection="",key={},condition={},initial={"count": 0},reduce="function(obj, prev){prev.count++;}"):
        cursor=self.db[collection].group(key,condition,initial,reduce)
        return cursor
    def get_collectionnames(self):
        return self.db.collection_names()
    
    def db_getDB(self):
        return self.db


def test():
    mdb = db_mongodb('183.57.42.116', 27017, "crawl_data")
    tt = mdb.db_find_one("chinaweath_20141016", {"cpdjbm":"C1010314005453"})
    print (tt)
    lt = mdb.db_group_by("news_content", {"main_id":1})
    print (lt)
# test()