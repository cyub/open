GO = go

EXECUTABLES = batchsumV1 batchsumV1Check batchsumV2 batchsumV2Check batchsumV3 batchsumV3Check\
	batchsumV4 batchsumV4Check batchsumV5 batchsumV5Check batchsumV6 batchsumV6Check
.PHONY:  $(EXECUTABLES)

all: $(EXECUTABLES)

batchsumV1:
	$(GO) build -o $@ $@.go

batchsumV1Check:
	$(GO) build -race -o $@ batchsumV1.go

batchsumV2:
	$(GO) build -o $@ $@.go

batchsumV2Check:
	$(GO) build -race -o $@ batchsumV2.go

batchsumV3:
	$(GO) build -o $@ $@.go

batchsumV3Check:
	$(GO) build -race -o $@ batchsumV3.go

batchsumV4:
	$(GO) build -o $@ $@.go

batchsumV4Check:
	$(GO) build -race -o $@ batchsumV4.go

batchsumV5:
	$(GO) build -o $@ $@.go

batchsumV5Check:
	$(GO) build -race -o $@ batchsumV5.go

batchsumV6:
	$(GO) build -o $@ $@.go

batchsumV6Check:
	$(GO) build -race -o $@ batchsumV5.go

clean:
	rm -f $(EXECUTABLES)



