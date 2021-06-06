__author__ = 'xkm'
#coding=utf-8
import redis
from redis.sentinel import Sentinel

class RedisHelperSL:
    def __init__(self,linkList,iDB,sPassword):
        self._sentinelLinkList=linkList
        # # 链接服务端
        # self.__pool=redis.ConnectionPool(host=sHost,
        #                   port=iPort,db=iDB,
        #                   password=sPassword)
        # 连接哨兵服务器(主机名也可以用域名)
        self._iDB=iDB
        self._sPassword=sPassword
        self.conn=None
        self.ps = None


    def runInit(self):
        # 连接哨兵服务器(主机名也可以用域名)
        self._sentinel = Sentinel(self._sentinelLinkList,socket_timeout=0.5)
        master = self._sentinel.discover_master('mymaster')
        self.__pool=redis.ConnectionPool(host=master[0],
                          port=master[1],db=self._iDB,
                          password=self._sPassword)
        self.conn=redis.StrictRedis(connection_pool=self.__pool)
        self.ps = self.conn.pubsub()

    def public(self, sTopicPub,msg):
        #发消息订阅方
        # publish发消息加入频道chan_pub
        self.conn.publish(sTopicPub, msg)
        return True

    def listenSub(self,topicList):
        # self.ps.subscribe('"'+'","'.join(topicList)+'"')
        self.ps.subscribe(topicList)
        #从foo，bar 订阅消息
        return self.ps

    def unListenSub(self,topicList):
        self.ps.unsubscribe(topicList)
        #从foo，bar 订阅消息
        return self.ps