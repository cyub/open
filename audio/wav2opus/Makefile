CC=gcc

BIN=wav2opus

DATASET=$(PWD)/../dataset
WAV_FILE=$(DATASET)/test.wav
OPUS_FILE=test.opus


build:
	$(CC) -o $(BIN) main.c  -lopus

libopus:
	sudo apt-get install libopus-dev

test:
	./$(BIN) $(WAV_FILE) $(OPUS_FILE)

clean:
	rm -rf $(BIN)
	rm -rf *.opus