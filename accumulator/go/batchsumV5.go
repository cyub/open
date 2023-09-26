package main

import (
	"fmt"
	"sync"
)

var n int64 = 1e6         // 计算从1到n整数和
var chunksize int64 = 1e4 // 分块计算，每块待计算数的个数
var sum int64             // 计算的结果

type Info struct {
	start int64 // 分块计算，块起始数字
	sum   int64 // 存储分块计算的结果
}

func add_numbers(wg *sync.WaitGroup, info *Info) {
	defer wg.Done()
	for i := info.start; i < info.start+chunksize && i <= n; i++ {
		info.sum += i
	}
}

func main() {
	var batch int64 // // 分几批次计算
	if n%chunksize == 0 {
		batch = n / chunksize
	} else {
		batch = n/chunksize + 1
	}

	var infos = make([]*Info, batch)
	for i := int64(0); i < batch; i++ {
		infos[i] = &Info{
			start: i*chunksize + 1,
			sum:   0,
		}
	}

	var wg sync.WaitGroup // 等待组，用于等待一组任务完成
	wg.Add(int(batch))

	var i int64
	for i = 0; i < batch; i++ {
		go add_numbers(&wg, infos[i]) // 创建协程处理批量计算
	}
	wg.Wait() // 等待所有协程完成

	for i := int64(0); i < batch; i++ {
		sum += infos[i].sum
	}
	fmt.Printf("sum: %d\n", sum)
	assert(sum, (n*(n+1))/2)
}

func assert(actual, expect int64) {
	if actual != expect {
		panic(fmt.Sprintf("%d != %d", actual, expect))
	}
}
