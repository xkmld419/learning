# coding:utf-8
#########################################
#
#author:
#company:hidata
#date:2014-6-10
#
#########################################

import Queue
from threading import Thread

STOP_COMMAND='S'
RUN_COMMAND='R'

#线程函数
class thread_func(Thread):
    #线程id,函数，输入队列，输出队列
    def __init__(self,th_id,func,Q_in,Q_out):
        Thread.__init__(self)
        self.th_id=th_id
        self.func=func
        self.Q_in=Q_in
        self.Q_out=Q_out
        self.Stop=False
        
    def run(self):
        while self.Stop is not True:
            command,arg=self.Q_in.get()
            if command==STOP_COMMAND:
                break
            elif command==RUN_COMMAND:
                try:
                    res=self.func(arg)
                    self.Q_out.put(res)
                except:
                    pass
    #主动停止线程
    def terminate(self):
        self.Stop=True
            
#线程池
class thread_pool(Thread):
    #线程数，函数，参数
    def __init__(self,num_threads,func,arg):
        Thread.__init__(self)
        self.Q_in=queue.Queue()
        self.Q_out=queue.Queue()
        self.num_threads=num_threads
        self.func=func
        self.arg=arg
        self.pool=[]
        for i in range(num_threads):
            thd=thread_func(i,func,self.Q_in,self.Q_out)
            self.pool.append(thd)
            thd.start()
    def run(self):
        size=len(self.arg)
        count=0
        #循环传参
        while count < size :
            self.Q_in.put((RUN_COMMAND,self.arg[count]))
            count=count+1
        #结束线程
        for thd in self.pool:
            self.Q_out.put((STOP_COMMAND,None))
               

def func(item):
    print(item)
    return item

def main():
    lst=['1','2','3','11','12','13','21','22','23']
    pool = thread_pool(3,func,lst)
    pool.start()
    pass


if __name__=='__main__':
    main()
