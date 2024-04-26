all: bounded_lock_queue

bounded_lock_queue: bounded_lock_queue.c
	$(CC) -g -o $@ $< -pthread

clean:
	rm -rf bounded_lock_queue