import threading


def function(i):
    print(f"i:{i}")


threads = []

for i in range(5):
    t = threading.Thread(target=function, args=(i,))
    threads.append(t)
    t.start()
    # t.join() 这个地方不能join,注意应把“启动”和“等待”分开，要不然串行化了

for t in threads:
    t.join()
