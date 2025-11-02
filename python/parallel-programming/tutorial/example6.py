import threading
import time


def first_function():
    print(threading.current_thread().name + "is starting")
    time.sleep(2)
    print(threading.current_thread().name + "is exiting")


def second_function():
    print(threading.current_thread().name + "is starting")
    time.sleep(2)
    print(threading.current_thread().name + "is exiting")


def third_function():
    print(threading.current_thread().name + "is starting")
    time.sleep(2)
    print(threading.current_thread().name + "is exiting")


if __name__ == "__main__":
    t1 = threading.Thread(target=first_function)
    t2 = threading.Thread(target=second_function)
    t3 = threading.Thread(name="custom_thread_name", target=third_function)

    t1.start()
    t2.start()
    t3.start()
    t1.join()
    t2.join()
    t3.join()
