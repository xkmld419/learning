__author__ = 'xkm'
#coding=utf-8
import redis

class RedisHelper:
    def __init__(self,sHost,iPort,iDB,sPassword):
        # 链接服务端
        self.__pool=redis.ConnectionPool(host=sHost,
                          port=iPort,db=iDB,
                          password=sPassword)
        self.conn=redis.StrictRedis(connection_pool=self.__pool)
        self.ps = self.conn.pubsub()

#
#
#         >>> r.set('name', 'jihite')
# True
# >>> r.set('score', 100)
# True
# >>> r.keys()
# ['score', 'name']
# >>> r.get('name')
# 'jihite'
# >>> r.get('score')
# '100'
# >>> r.delete('score')
# 1
# >>> r.keys()
# ['name']
# >>> r.save()
# True
# >>> r.keys()
# ['name']
# >>> r.flushdb()
# True
# >>> r.keys()
# []

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