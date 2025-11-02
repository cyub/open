from concurrent.futures import ThreadPoolExecutor, as_completed, wait
import time


def job(x):
    time.sleep(x)
    return x * x


if __name__ == "__main__":
    # 流式获取feture结果
    with ThreadPoolExecutor(max_workers=4) as executor:
        futures = [executor.submit(job, i) for i in range(1, 6)]  # 1~5 秒的任务

        for fut in as_completed(futures):  # 先完成先返回
            print("got result:", fut.result())

    # 使用wait一次获取所有结果
    with ThreadPoolExecutor(max_workers=4) as executor:
        futures = [executor.submit(job, i) for i in range(5)]

        # 阻塞到全部完成
        wait(futures, timeout=10)  # 返回 (done, not_done) 二元组，这里忽略

        # 再按原始顺序拿结果
        results = [f.result() for f in futures]
        print(results)

    # 使用map一次性获取所有结果
    with ThreadPoolExecutor(max_workers=4) as executor:
        results = list(executor.map(job, range(5)))  # 阻塞直到全部完成
        print(results)
