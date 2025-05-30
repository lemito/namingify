package main

import (
	"context"
	"fmt"
	"runtime"
	"sync"
	"sync/atomic"
	"time"
)

func work(ctx context.Context, id int, input <-chan int, res chan<- string, wg *sync.WaitGroup) {
	defer wg.Done()

	for {
		select {
		case <-ctx.Done():
			{
				return
			}
		case in, status := <-input:
			{
				if !status {
					return
				}

				time.Sleep(time.Microsecond * 100)

				pre_res := fmt.Sprintf("This %d was done by %d", in, id)

				res <- pre_res

			}
		}
	}
}

func with_atomic(cnt *atomic.Int32) {
	cnt.Add(1)
}

func once_work(once *sync.Once) {
	once.Do(func() {
		fmt.Println("Start")
	})
}

func main() {

	const (
		workers = 7
		cnt     = 20
	)

	inpts := make(chan int, cnt)
	res := make(chan string, cnt)

	var wg sync.WaitGroup
	var once sync.Once
	var meow atomic.Int32
	meow.Store(0)

	for i := 1; i <= cnt; i++ {
		go once_work(&once)
	}

	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	for i := 1; i <= cnt; i++ {
		wg.Add(1)
		go work(ctx, i, inpts, res, &wg)
	}

	for i := 1; i <= cnt; i++ {
		inpts <- (i + 27/4 + 1)
	}
	close(inpts)

	go func() {
		for result := range res {
			fmt.Println(result)
		}
	}()

	wg.Wait()
	close(res)

	var wg2 sync.WaitGroup

	for i := 1; i <= cnt; i++ {
		wg2.Add(1)
		go func() {
			defer wg2.Done()
			with_atomic(&meow)
		}()
	}

	wg2.Wait()

	fmt.Printf("Atomic = %d\n", meow.Load())
	fmt.Println("================")
	fmt.Printf("GOMAXPROCS: %d\n", runtime.GOMAXPROCS(0))
	fmt.Printf("Final goroutines: %d\n", runtime.NumGoroutine())
}
