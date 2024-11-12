CC=gcc

BIN=wav2vorbis

WAV_FILE=../dataset/test.wav

build:
	$(CC) -o $(BIN) main.c  -lopus -logg -lvorbis -lsndfile -lvorbisenc

libopus:
	sudo apt-get install libopus-dev libogg-dev libvorbis-dev libsndfile1-dev

test:
	./$(BIN) $(WAV_FILE)

clean:
	rm -rf $(BIN)
	rm -rf *.ogg