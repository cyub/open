# 计算从1到n自然数之和

分批次并行计算，每批次处理相同数量的数。比如计算1到100的和，分10批次计算，那么第一次批次处理的数是1到10，第二批次处理的是10到20，依次类推。

## 计数器实现优化记录

版本 | 优缺点
--- | ---
[batchsumV1](./batchsumV1.c) | 数据竞态导致并发问题
[batchsumV2](./batchsumV2.c) | 基于锁解决并发问题，但锁次数使用太多
[batchsumV3](./batchsumV3.c) | 相比V2进一步优化，减少了锁使用次数
[batchsumV4](./batchsumV4.c) | 相比V3进一步优化，基于原子操作实现，避免使用到锁
[batchsumV5](./batchsumV5.c)   | 相比V5进一步优化，基于线程局部变量（类似线程局部存储机制），避免原子操作