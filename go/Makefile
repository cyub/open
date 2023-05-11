GO = go

EXECUTABLES = batchsumV1 batchsumV1Check batchsumV2 batchsumV2Check
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

clean:
	rm -f $(EXECUTABLES)



