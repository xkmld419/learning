#coding=utf-8


import queue
import sys
from threading import Thread
import time

#线程池控制类，设置最大的队列数（300），队列为空时不退出，需要外部调用结束指令才会退出，返回结果单独存放在
#out队列中，提交接口外部调用

# working thread
class Worker(Thread):
    worker_count = 0
    def __init__(self, workQueue, resultQueue, timeout=0, auto_stop=False, **kwds):
        Thread.__init__(self, **kwds)
        self.id = Worker.worker_count
        Worker.worker_count += 1
        self.setDaemon(True)
        self.workQueue = workQueue
        self.resultQueue = resultQueue
        self.timeout = timeout
        self.isRunning = True
        self.auto_stop = auto_stop
        self.setName(self.id)
        self.exist_count = 5
        self.start()

    def run(self):
        #the get-some-work, do-some-work main loop of worker threads
        while True and self.isRunning:
            try:
                callable, args = self.workQueue.get(timeout=self.timeout)
                res = callable(*args)
                if res is not None:
                    self.resultQueue.put(res)
                    
            except queue.Empty:
                # print str(self.id) + "sleep"
                time.sleep(10)
                self.exist_count -= 1
                if self.auto_stop and self.exist_count == 0:
                    break
                else:
                    continue
            except:
                print('worker[%2d]' % self.id, sys.exc_info()[:2])
        print("running:" + str(self.id))

    # def thread_start(self):
    #     self.start()
        # self.isRunning = True

    def thread_stop(self):
        self.isRunning = False

#线程池管理类
class WorkerManager(Thread):
    #num_of_workers为线程数，默认为10个
    def __init__(self, num_of_workers=10, resultQueue=None, timeout=1, auto_stop=False, queue_size=30):
        Thread.__init__(self)
        # 设置工作线程队列最大值为300
        self.workQueue = queue.Queue(queue_size)
        if resultQueue is None:
            self.resultQueue = queue.Queue()
        else:
            self.resultQueue = resultQueue
        self.workers = []
        self.timeout = timeout
        self.num_of_workers = num_of_workers
        self.auto_stop = auto_stop
        self._recruitThreads(num_of_workers)
        self.joiningWorker = None
        self.start()

    #创建工作线程
    def _recruitThreads(self, num_of_workers):
        for i in range(num_of_workers):
            worker = Worker(self.workQueue, self.resultQueue, self.timeout, auto_stop=self.auto_stop)
            self.workers.append(worker)

    #等待线程结束，当队列中的全部项都被执行完后，退出
    def wait_for_complete(self):
        #启动线程池内所有线程
        # self._threads_start()
        # ...then, wait for each of them to terminate:
        
        while len(self.workers):
            worker = self.workers.pop()
            self.joiningWorker = worker
            worker.join()
            if worker.isAlive() and not self.workQueue.empty():
                self.workers.append(worker)
            elif not worker.isAlive() and not self.workQueue.empty():
                worker = Worker(self.workQueue, self.resultQueue, self.timeout, auto_stop=self.auto_stop)
                self.workers.append(worker)
                
        print("All jobs are completed.")

    def run(self):
        self.wait_for_complete()

    #把一项任务加到队列中
    def add_job(self, callable, *args):
        self.workQueue.put((callable, args))

    #外部调用，用来停止线程池
    def thread_stop(self):
        for worker in self.workers:
            worker.thread_stop()

        # pop出来的workder不在workers队列内，需要单独释放
        self.joiningWorker.thread_stop()

    #外部调用，用来获取out队列中的数据，out队列也就是每项任务的返回值
    def get_result(self, *args, **kwds):
        result = None
        try:
            result = self.resultQueue.get(timeout=self.timeout)
            print("result size" + str(self.resultQueue.qsize()))
        except queue.Empty:
            time.sleep(1)
        except:
            print('get_result: ', sys.exc_info()[:2])

        return result

    # def _threads_start(self):
    #     for i in range(0, self.num_of_workers, 1):
    #         self.workers[i].thread_start()
