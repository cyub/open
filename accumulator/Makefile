CC = gcc

EXECUTABLES = batchsumV1 batchsumV1Check batchsumV2 batchsumV2Check \
	batchsumV3 batchsumV3Check batchsumV4 batchsumV4Check batchsumV5 batchsumV5Check

.PHONY:  $(EXECUTABLES)

all: $(EXECUTABLES)

fib:
	$(CC) $@.c -o $@

batchsumV1:
	$(CC) -g $@.c -o $@

batchsumV1Check:
	$(CC) -Wall -fsanitize=thread -g batchsumV1.c -o $@

batchsumV2:
	$(CC) $@.c -o $@

batchsumV2Check:
	$(CC) -Wall -fsanitize=thread -g batchsumV2.c -o $@


batchsumV3:
	$(CC) -Wall $@.c -o $@ -lpthread

batchsumV3Check:
	$(CC) -Wall  -fsanitize=thread batchsumV3.c -o $@ -lpthread

batchsumV4:
	$(CC) -Wall $@.c -o $@ -lpthread

batchsumV4Check:
	$(CC) -Wall -fsanitize=thread batchsumV4.c -o $@ -lpthread

batchsumV5:
	$(CC) -Wall $@.c -o $@ -lpthread

batchsumV5Check:
	$(CC) -Wall -fsanitize=thread batchsumV5.c -o $@ -lpthread

clean:
	rm -f $(EXECUTABLES)



