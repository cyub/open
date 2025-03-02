package syncutil

import (
	"context"
	"sync"
	"sync/atomic"
	"time"
)

type CompletionSignal struct {
	doneChan  chan struct{}
	completed int32
}

func NewCompletionSignal() *CompletionSignal {
	return &CompletionSignal{
		doneChan:  make(chan struct{}),
		completed: 0,
	}
}

func (s *CompletionSignal) Done() {
	if atomic.SwapInt32(&s.completed, 1) == 0 {
		close(s.doneChan)
	}
}

func (s *CompletionSignal) IsCompleted() bool {
	return atomic.LoadInt32(&s.completed) == 1
}

func (s *CompletionSignal) Wait() <-chan struct{} {
	return s.doneChan
}

func (s *CompletionSignal) WaitBlocking() {
	<-s.doneChan
}

func (s *CompletionSignal) WaitWithTimeout(timeout time.Duration) bool {
	if timeout <= 0 {
		return s.IsCompleted()
	}

	if s.IsCompleted() {
		return true
	}

	select {
	case <-s.doneChan:
		return true
	case <-time.After(timeout):
		return s.IsCompleted()
	}
}

func (s *CompletionSignal) WaitUntil(deadline time.Time) bool {
	timeout := time.Until(deadline)
	if timeout <= 0 {
		return s.IsCompleted()
	}
	return s.WaitWithTimeout(timeout)
}

func (s *CompletionSignal) Context() context.Context {
	ctx, cancel := context.WithCancel(context.Background())
	go func() {
		<-s.Wait()
		cancel()
	}()
	return ctx
}

func WaitAny(signals ...*CompletionSignal) *CompletionSignal {
	combined := NewCompletionSignal()
	for _, s := range signals {
		go func(sig *CompletionSignal) {
			<-sig.Wait()
			combined.Done()
		}(s)
	}
	return combined
}

func WaitAll(signals ...*CompletionSignal) *CompletionSignal {
	combined := NewCompletionSignal()
	var wg sync.WaitGroup
	wg.Add(len(signals))
	for _, sig := range signals {
		go func(s *CompletionSignal) {
			<-s.Wait()
			wg.Done()
		}(sig)
	}
	go func() {
		wg.Wait()
		combined.Done()
	}()
	return combined
}
