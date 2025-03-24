package syncutil

import "sync/atomic"

type AtomicBool struct {
    val int32
}

func NewAtomicBool(initial bool) *AtomicBool {
    ab := &AtomicBool{}
    ab.Store(initial)
    return ab
}

func (ab *AtomicBool) Load() bool {
    return atomic.LoadInt32(&ab.val) != 0
}

func (ab *AtomicBool) Store(value bool) {
    var v int32 = 0
    if value {
        v = 1
    }
    atomic.StoreInt32(&ab.val, v)
}

func (ab *AtomicBool) Swap(new bool) (old bool) {
    var newVal int32 = 0
    if new {
        newVal = 1
    }
    oldVal := atomic.SwapInt32(&ab.val, newVal)
    return oldVal != 0
}

func (ab *AtomicBool) CompareAndSwap(old, new bool) bool {
    var oldVal, newVal int32 = 0, 0
    if old {
        oldVal = 1
    }
    if new {
        newVal = 1
    }
    return atomic.CompareAndSwapInt32(&ab.val, oldVal, newVal)
}