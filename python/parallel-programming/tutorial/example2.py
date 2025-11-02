import concurrent.futures
import time


def worker(index):
    print(f"Thread {index} is starting")
    time.sleep(2)
    print(f"Thread {index} is done")


start = time.time()
with concurrent.futures.ProcessPoolExecutor(max_workers=3) as executor:
    futures = [executor.submit(worker, i) for i in range(5)]

concurrent.futures.wait(futures)

end = time.time()
print("concurrent cost: ", end - start, " seconds")
print("All threads are done")
