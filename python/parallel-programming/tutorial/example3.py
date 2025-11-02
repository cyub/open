import asyncio
import aiohttp


async def fetch_url(session, url):
    async with session.get(url) as response:
        return await response.text()


async def main():
    urls = ["https://www.baidu.com", "https://www.cyub.vip"]

    async with aiohttp.ClientSession() as session:
        tasks = [fetch_url(session, url) for url in urls]

        results = await asyncio.gather(*tasks)

        for url, result in zip(urls, results):
            print(f"Content from {url}: {len(result)} bytes")


asyncio.run(main())
