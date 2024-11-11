CC=gcc

BIN=wav2opus

WAV_FILE=test.wav

build:
	$(CC) -o $(BIN) main.c  -lopus

libopus:
	sudo apt-get install libopus-dev

test:
	./$(BIN) $(WAV_FILE)

clean:
	rm -rf $(BIN)
	rm -rf *.opus