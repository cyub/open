
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

long n = 1e6;          // 计算从1到n整数和
long chunksize = 1e4;  // 分块计算，每块待计算数的个数
long sum;              // 计算的结果
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *add_numbers(void *arg) {
  long i;
  long start = *(long *)arg;
  pthread_mutex_lock(&m);
  for (i = start; i < start + chunksize && i <= n; i++) {
    sum += i;
  }
  pthread_mutex_unlock(&m);
}

int main() {
  long batch = n % chunksize == 0 ? (n / chunksize)
                                  : ((n / chunksize) + 1);  // 分几批次计算

  long *starts =
      (long *)(malloc(sizeof(long) * batch));  // 记录每批次的起始的那个数
  for (long i = 0; i < batch; i++) {
    starts[i] = i * chunksize + 1;
  }

  pthread_t *threads = (pthread_t *)(malloc(sizeof(pthread_t) * batch));
  for (long i = 0; i < batch; i++) {  // 使用线程进行批次并发处理
    pthread_create(&threads[i], NULL, add_numbers, (void *)(starts + i));
  }

  for (long i = 0; i < batch; i++) {
    pthread_join(threads[i], NULL);  // 等待所有线程完成工作
  }

  printf("sum: %ld\n", sum);
  long expect = (n * (n + 1)) / 2;
  assert(sum == expect);
}