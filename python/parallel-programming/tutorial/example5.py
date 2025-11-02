import threading
import time


def function(i):
    time.sleep(1)
    print(f"i:{i}")


threads = [threading.Thread(target=function, args=(i,)) for i in range(5)]

for t in threads:
    t.start()

for t in threads:
    t.join()
