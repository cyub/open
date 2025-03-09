package syncutil

import (
	"fmt"
	"sync"
	"time"
)

type ResettableTimer struct {
	timer   *time.Timer
	resetCh chan time.Duration
	stopCh  chan struct{}
	once    sync.Once
}

func NewResettableTimer() *ResettableTimer {
	t := &ResettableTimer{
		resetCh: make(chan time.Duration),
		stopCh:  make(chan struct{}),
		timer:   time.NewTimer(1<<63 - 1), // 设置超长时间，避免立即触发
	}
	go t.timerLoop()
	return t
}

func (t *ResettableTimer) timerLoop() {
	for {
		select {
		case <-t.timer.C:
			return
		case newDuration := <-t.resetCh:
			t.drainTimer()
			t.timer.Reset(newDuration)
		case <-t.stopCh:
			t.drainTimer()
			return
		}
	}
}

func (t *ResettableTimer) drainTimer() {
	select {
	case <-t.timer.C:
	default:
	}
}

func (t *ResettableTimer) Reset(duration time.Duration) {
	select {
	case t.resetCh <- duration:
	default:
	}
}

func (t *ResettableTimer) Stop() {
	t.once.Do(func() {
		close(t.stopCh)
	})
}
