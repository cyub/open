package main

import (
	"bytes"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"unsafe"
)

/*
#cgo pkg-config: opus
#include <opus/opus.h>

int opus_encoder_set_bitrate(OpusEncoder *st, int bitrate) {
    return opus_encoder_ctl(st, OPUS_SET_BITRATE(bitrate));
}

int opus_encoder_set_vbr(OpusEncoder *st, int vbr) {
    return opus_encoder_ctl(st, OPUS_SET_VBR(vbr));
}
*/
import "C"

type WavHeader struct {
	ChunkID       [4]byte // "RIFF"
	ChunkSize     uint32  // 文件大小 - 8
	Format        [4]byte // "WAVE"
	Subchunk1ID   [4]byte // "fmt "
	Subchunk1Size uint32  // fmt子块大小，PCM为16
	AudioFormat   uint16  // 音频格式，PCM为1
	NumChannels   uint16  // 通道数
	SampleRate    uint32  // 采样率
	ByteRate      uint32  // 字节率
	BlockAlign    uint16  // 块对齐
	BitsPerSample uint16  // 位深度
	Subchunk2ID   [4]byte // "data"
	Subchunk2Size uint32  // 音频数据的大小
}

func main() {
	var (
		wavfile  string
		opusfile string
		bitrate  int
	)
	flag.StringVar(&wavfile, "i", "", "wav file")
	flag.StringVar(&opusfile, "o", "", "opus file")
	flag.IntVar(&bitrate, "b", 32000, "opus bitrate")
	flag.Parse()

	if len(wavfile) == 0 || len(opusfile) == 0 {
		flag.Usage()
		os.Exit(1)
	}

	f, err := os.Open(wavfile)
	if err != nil {
		log.Printf("Open wav file error: %v", err)
		return
	}
	defer f.Close()

	wavHeader, err := readWavHeader(f)
	if err != nil {
		log.Printf("Read wav header error: %v", err)
		return
	}

	if !bytes.Equal(wavHeader.ChunkID[:], []byte("RIFF")) || !bytes.Equal(wavHeader.Format[:], []byte("WAVE")) {
		log.Printf("Illegal wav foramt")
		return
	}

	if wavHeader.AudioFormat != 1 {
		log.Printf("Only support wav pcm format")
		return
	}

	var errCode C.int
	encoder := C.opus_encoder_create(C.opus_int32(wavHeader.SampleRate), C.int(wavHeader.NumChannels), C.OPUS_APPLICATION_AUDIO, &errCode)
	if errCode != C.OPUS_OK {
		log.Printf("Opus encoder create error: %s", C.GoString(C.opus_strerror(errCode)))
		return
	}
	defer C.opus_encoder_destroy(encoder)

	errCode = C.opus_encoder_set_bitrate(encoder, C.int(bitrate))
	if errCode != C.OPUS_OK {
		log.Printf("Opus encoder set bit rate error: %s", C.GoString(C.opus_strerror(errCode)))
		return
	}

	errCode = C.opus_encoder_set_vbr(encoder, C.int(0))
	if errCode != C.OPUS_OK {
		log.Printf("Opus encoder set vbr error: %s", C.GoString(C.opus_strerror(errCode)))
		return
	}

	frameSize := int(wavHeader.SampleRate/1000) * 20
	opusFile, err := os.Create(opusfile)
	if err != nil {
		log.Printf("Create opus file error: %v", err)
		return
	}
	defer opusFile.Close()

	pcmData := make([]byte, frameSize*int(wavHeader.NumChannels)*int(wavHeader.BitsPerSample/8))
	opusData := make([]byte, len(pcmData))

	for {
		n, err := f.Read(pcmData)
		if err != nil && err != io.EOF {
			log.Printf("Read file error: %v", err)
			return
		}
		if err == io.EOF && n <= 0 {
			break
		}

		opusLen := C.opus_encode(encoder, (*C.opus_int16)(unsafe.Pointer(&pcmData[0])), C.int(frameSize), (*C.uchar)(unsafe.Pointer(&opusData[0])), C.opus_int32(len(opusData)))
		if opusLen < 0 {
			log.Printf("Opus encode error: %v", C.GoString(C.opus_strerror(errCode)))
			return
		}

		_, err = opusFile.Write(opusData[:int(opusLen)])
		if err != nil {
			log.Printf("Write opus file error: %v", err)
			return
		}
	}

	fmt.Printf("WAV文件%s已成功转换为Opus文件%s\n", wavfile, opusfile)
}

func readWavHeader(r io.Reader) (WavHeader, error) {
	var wavHeader WavHeader
	if err := binary.Read(r, binary.LittleEndian, &wavHeader); err != nil {
		return wavHeader, err
	}
	return wavHeader, nil
}
