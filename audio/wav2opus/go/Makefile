GO = go
BIN = wav2opus
DATASET = $(PWD)/../../dataset

all: build

install:
	sudo apt-get install -y pkg-config libopus-dev

build:
	$(GO) build -o $(BIN) main.go

test:
	./$(BIN) -i $(DATASET)/test.wav -o test.opus
	
clean:
	rm -rf $(BIN)
	rm -rf *.opus