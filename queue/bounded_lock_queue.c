/*
 * 基于ringbuffer结构实现的有界阻塞队列
 * 队列元素类型为int
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int front;
  int rear;
  int count;
  int capacity;
  int *elements;
  pthread_mutex_t mutex;
  pthread_cond_t not_full;
  pthread_cond_t not_empty;
} bounded_lock_queue;

void bounded_lock_queue_init(bounded_lock_queue *q, int capacity) {
  q->capacity = capacity;
  q->elements = calloc(capacity, sizeof(int));
  q->front = q->rear = q->count = 0;
  pthread_mutex_init(&q->mutex, NULL);
  pthread_cond_init(&q->not_full, NULL);
  pthread_cond_init(&q->not_empty, NULL);
}

void bounded_lock_queue_enq(bounded_lock_queue *q, int v) {
  pthread_mutex_lock(&q->mutex);
  while (q->count == q->capacity) {
    pthread_cond_wait(&q->not_full, &q->mutex);
  }
  q->elements[q->rear] = v;
  if (++q->rear == q->capacity) {
    q->rear = 0;
  }
  q->count++;
  pthread_cond_signal(&q->not_empty);
  pthread_mutex_unlock(&q->mutex);
}

int bounded_lock_queue_deq(bounded_lock_queue *q) {
  pthread_mutex_lock(&q->mutex);
  while (q->count == 0) {
    pthread_cond_wait(&q->not_empty, &q->mutex);
  }
  int v = q->elements[q->front];
  if (++q->front == q->capacity) {
    q->front = 0;
  }
  q->count--;
  pthread_cond_signal(&q->not_full);
  pthread_mutex_unlock(&q->mutex);
  return v;
}

void *produce(void *arg) {
  bounded_lock_queue *boundedLockQueue = (bounded_lock_queue *)arg;
  int n = pthread_self() % 1000;
  for (int i = n; i < n + 10; i++) {
    bounded_lock_queue_enq(boundedLockQueue, i);
  }
}

void *consume(void *arg) {
  bounded_lock_queue *boundedLockQueue = (bounded_lock_queue *)arg;
  int v;
  for (int i = 0; i < 10; i++) {
    v = bounded_lock_queue_deq(boundedLockQueue);
    printf("deq: %d\n", v);
  }
}

void main() {
  bounded_lock_queue bounedLockQueue;
  bounded_lock_queue_init(&bounedLockQueue, 5);
  pthread_t producers[2];
  pthread_t consumers[2];

  for (int i = 0; i < 2; i++) {
    pthread_create(&producers[i], NULL, produce, &bounedLockQueue);
  }

  for (int i = 0; i < 2; i++) {
    pthread_create(&consumers[i], NULL, consume, &bounedLockQueue);
  }
  pthread_join(producers[0], NULL);
  pthread_join(producers[1], NULL);
  pthread_join(consumers[0], NULL);
  pthread_join(consumers[1], NULL);
}