原因一句话  
**CPython 的 GIL（全局解释器锁）在“字节码调度”层面保护了 `+=`/`-=` 的原子性**，导致两个线程交替执行 **一整条字节码** 时几乎不会交错；于是即使不加锁，也大概率得到 0，看上去“没有竞态”。

------------------------------------------------
细节拆解

1. `x += 1` 在 CPython 3.11 之前对应一条 **字节码** `BINARY_ADD`（或 `INPLACE_ADD`），GIL 在同一时刻只允许一个线程**完整**执行一条字节码；  
   因此“读-改-写”被隐式地串行化了，竞态窗口极窄。

2. 循环次数虽然大（1 亿），但**每次循环仍然只触发一次字节码**，所以交错概率低到在你的几次实验里没出现。

3. 把一条增量拆成**多条**字节码就能让竞态暴露，例如：

```python
def increment_without_lock():
    global shared_resource_with_no_lock
    for _ in range(COUNT):
        v = shared_resource_with_no_lock   # 读
        v += 1                              # 改
        shared_resource_with_no_lock = v    # 写
```

运行后几乎总会得到非 0 的随机值。

4. 官方文档也提到：  
   > “The GIL is not a safeguard against race conditions in your code.  
   > It only protects the interpreter’s internal data structures.”  
   换句话说，**GIL 不保证业务级原子性**，只是你恰好撞上了“字节码级原子”的特例。

------------------------------------------------
如何肉眼看到竞态

- 把 `COUNT` 调大到 `10_000_000` 并**多跑几遍**；  
- 或者如上把 `+=` 拆成三行；  
- 或者换 **PyPy**、**无 GIL 的 Python 3.12 `--disable-gil`** 版本，立刻能看到非 0 结果。

结论  
不是代码有问题，而是 **CPython 的 GIL 让这条字节码变得“看似原子”**；想演示竞态，需要把临界区拆成多条字节码或换个解释器。