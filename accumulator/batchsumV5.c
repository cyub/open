#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

long n = 1e6;          // 计算从1到n整数和
long chunksize = 1e4;  // 分块计算，每块待计算数的个数
long sum;              // 计算的结果

typedef struct {
  long start;  // 分块计算，块起始数字
  long sum;    // 存储分块计算的结果
} Info;

void *add_numbers(void *arg) {
  long i;
  Info *info = (Info *)arg;
  long start = info->start;
  long end = info->start + chunksize <= n ? info->start + chunksize : n + 1;
  for (i = start; i < end; i++) {
    info->sum += i;
  }

  return NULL;
}

int main() {
  long batch = (n + chunksize - 1) / chunksize;  // 分几批次计算
  long numThreads = sysconf(_SC_NPROCESSORS_ONLN);
  if (numThreads <= 0) numThreads = 4;
  // 限制线程数等于CPU核数，避免线程过多造成频繁上下文切换开销
  batch = numThreads < batch ? numThreads : batch;

  Info *infos = (Info *)(malloc(sizeof(Info) * batch));
  for (long i = 0; i < batch; i++) {
    infos[i].start = i * chunksize + 1;
    infos[i].sum = 0;
  }

  pthread_t *threads = (pthread_t *)(malloc(sizeof(pthread_t) * batch));
  for (long i = 0; i < batch; i++) {  // 使用线程进行批次并发处理
    pthread_create(&threads[i], NULL, add_numbers, (void *)&infos[i]);
  }

  for (long i = 0; i < batch; i++) {
    pthread_join(threads[i], NULL);  // 等待所有线程完成工作
  }

  for (int i = 0; i < batch; i++) {
    sum += infos[i].sum;  // 汇总分块计算的结果
  }
  printf("sum: %ld\n", sum);
  long expect = (n * (n + 1)) / 2;
  assert(sum == expect);
}