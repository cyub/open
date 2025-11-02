from threading import Thread
from queue import Queue
import time
import random

POISON = object()  # 全局唯一的结束标记


class Producer(Thread):
    def __init__(self, queue: Queue, pill: object):
        super().__init__()
        self.queue = queue
        self.pill = pill

    def run(self):
        for _ in range(10):
            item = random.randint(0, 256)
            self.queue.put(item)
            print(f"Producer notify: item {item} appended by {self.name}")
            time.sleep(1)

        # 生产结束，发放毒丸
        for _ in range(N_CONSUMERS):  # 有多少消费者就发多少粒
            self.queue.put(self.pill)
        print("Producer finished.")


class Consumer(Thread):
    def __init__(self, queue: Queue, pill: object):
        super().__init__()
        self.queue = queue
        self.pill = pill

    def run(self):
        while True:
            item = self.queue.get()
            if item is self.pill:  # 遇到毒丸就自杀
                print(f"{self.name} received poison pill and exits.")
                self.queue.task_done()
                break
            print(f"Consumer notify: {item} popped by {self.name}")
            self.queue.task_done()


if __name__ == "__main__":
    N_CONSUMERS = 3
    q = Queue()
    producers = [Producer(q, POISON)]
    consumers = [Consumer(q, POISON) for _ in range(N_CONSUMERS)]

    for t in producers + consumers:
        t.start()

    # 等生产者先结束
    for p in producers:
        p.join()

    # 等队列里所有任务（包括毒丸）被处理完
    q.join()

    # 此时所有消费者已收到毒丸并退出，主线程再 join 它们
    for c in consumers:
        t.join()

    print("All done.")
