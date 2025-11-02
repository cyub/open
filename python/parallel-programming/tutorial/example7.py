import threading
import time


class myThread(threading.Thread):
    def __init__(self, threadID, name):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = name

    def run(self):
        print("Starting " + self.name)
        time.sleep(1)
        print("Exiting " + self.name)


if __name__ == "__main__":
    t1 = myThread(1, "Thread-1")
    t2 = myThread(2, "Thread-2")
    t1.start()
    t2.start()
    t1.join()
    t2.join()
