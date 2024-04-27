# Queue

## Ringbuffer

Ringbuffer入队过程：

![](../images/Ringbuffer.svg)

从上图中，可以看出当队列为空或者队列满的时候，都是front = rear = 0。

### 解决方案

#### 数据计数法

总是记录队列中数据元素个数。缺点是读写操作都需要修改这个存储数据计数，对于多线程访问缓冲区需要并发控制。

```c
typedef struct {
    int front;
    int rear;
    int count;
    int capacity;
    int *elements;
} ringbuffer

bool isEmpty() {
    return q->count == 0;
}

bool isFull() {
    return q->count == q->capacity;
}

int enqueue(int v) {
    if (isFull()) {
        printf("队列已满");
        return -1
    }
    q->elements[q->rear] = v;
    if (++q->rear == q->capacity) {
        q->rear = 0; // 回绕到队列头部
    }
    q->count++; // 更新数据计数
    return 0;
}

int dequeue() {
    if (isEmpty()) {
        printf("空队列");
        return -1
    }
    int v = q->elements[q->front];
    if (++q->front == q->capacity) {
        q->front = 0; // 回绕到队列头部
    }
    q->count--; // 更新数据计数
    return v;
}

int queue_length() {
    return q->count;
}
```

### 空余单元法

总是保持一个存储单元为空，也就是说队列缓冲区最多存入 `q->capacity - 1` 个数据。**如果读写指针指向同一位置，则缓冲区为空**。**如果写指针位于读指针的相邻后一个位置，则缓冲区为满**。这种策略的优点是简单、鲁棒；缺点是语义上实际可存数据量与缓冲区容量不一致，测试缓冲区是否满需要做取余数计算。

```c
typedef struct {
    int front;
    int rear;
    int capacity;
    int *elements;
} ringbuffer

bool isEmpty() {
    return q->front == q->rear;
}

bool isFull() {
    return (q->rear + 1)%q->capacity == q->front;
} 

int enqueue(int v) {
    if (isFull()) {
        printf("队列已满");
        return -1
    }
    q->elements[q->rear] = v;
    q->rear = (q->rear + 1)%q->capacity;
}

int dequeue(int v) {
    if (isEmpty()) {
        printf("空队列");
        return -1
    }
    int v = q->elements[q->front];
    q->front = (q->front + 1) % q->capacity;
    return v;
}

int queue_length() {
    return (q->rear - q->front + q->capacity) % q->capacity;
}
```

### 记录最后的操作类型

使用一位记录最后一次操作是读还是写。读写指针值相等情况下，如果最后一次操作为写入，那么缓冲区是满的；如果最后一次操作为读出，那么缓冲区是空。 这种策略的缺点是读写操作共享一个标志位，多线程时需要并发控制。

### 镜像指示位
略。

## 参考资料

- [百度百科：环形缓冲器](https://baike.baidu.com/item/%E7%8E%AF%E5%BD%A2%E7%BC%93%E5%86%B2%E5%99%A8/22701730)
- [百度百科：循环队列](https://baike.baidu.com/item/%E5%BE%AA%E7%8E%AF%E9%98%9F%E5%88%97/3685773)