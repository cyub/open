import asyncio
import time


def mark_done(fut: asyncio.Future) -> None:
    """由‘外部回调’在 2 秒后给 future 设置结果"""
    time.sleep(2)
    fut.set_result("Future is done!")


async def main():
    loop = asyncio.get_running_loop()

    # 1. 创建一个裸 Future
    fut = loop.create_future()  # 等价于 asyncio.Future(loop=loop)

    # 2. 启动一个线程去“模拟外部事件”完成 Future
    #    （真实场景里可以是网络库、文件监听等回调）
    loop.run_in_executor(None, mark_done, fut)

    # 3. 在这里挂起，直到 Future 被 set_result
    print("awaiting future...")
    result = await fut  # 挂起点
    print("got result:", result)


if __name__ == "__main__":
    asyncio.run(main())
