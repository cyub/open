from threading import Thread, Condition
import time

items = []
condition = Condition()


class consumer(Thread):
    def __init__(self):
        Thread.__init__(self)

    def consume(self):
        global condition
        global items
        condition.acquire()
        while len(items) == 0:
            condition.wait()
        items.pop()
        print("consumer consumed 1 item")
        print("consumer consume items len:" + str(len(items)))
        condition.notify_all()
        condition.release()

    def run(self):
        for i in range(0, 20):
            time.sleep(2)
            self.consume()


class producer(Thread):

    def __init__(self):
        Thread.__init__(self)

    def produce(self):
        global condition
        global items
        condition.acquire()
        while len(items) == 10:
            condition.wait()  # 内部先解锁 → 挂起 → 被唤醒后重新加锁
            print("producer producted are " + str(len(items)))

        items.append(1)
        print("producer items producted " + str(len(items)))
        condition.notify_all()
        condition.release()

    def run(self):
        for i in range(0, 20):
            time.sleep(1)
            self.produce()


if __name__ == "__main__":
    producer = producer()
    consumer = consumer()
    producer.start()
    consumer.start()
    producer.join()
    consumer.join()
