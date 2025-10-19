package main

import (
	"bytes"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"time"
)

const (
	sampleRate   = 16000 // 采样率 8kHz
	frameMs      = 20    // 每帧 20ms
	frameSamples = sampleRate * frameMs / 1000
	frameBytes   = frameSamples * 2 // 16bit PCM，每个样本2字节
	frameSize    = sampleRate / 1000 * frameMs
	channels     = 1 // 单声道

)

func main() {
	var inputFile string // wav单声道音频
	flag.StringVar(&inputFile, "input", "", "wav单声道音频")
	flag.Parse()

	if len(inputFile) == 0 {
		log.Fatal("input file not allow empty")
	}
	outDir := "segments"
	os.MkdirAll(outDir, 0755)

	file, err := os.Open(inputFile)
	if err != nil {
		panic(err)
	}
	file.Seek(44, io.SeekStart)
	defer file.Close()

	r, w := io.Pipe()
	go func() {
		var buff = make([]byte, 8192)
		for {
			n, err := file.Read(buff)
			if err != nil {
				if err == io.EOF {
					w.Close()
					break
				} else {
					panic(err)
				}
			}

			w.Write(buff[:n])
		}
	}()

	segmenter, err := NewVadSegmenter(VadConfig{
		SampleRate:     sampleRate,
		FrameDuration:  20,
		HangoverFrame:  15,
		MinSpeechFrame: 20,
		Model:          3,
	})
	if err != nil {
		panic(err)
	}

	var i = 0
	for seg := range segmenter.Process(r) {
		i++
		pcmPath := fmt.Sprintf("%s/seg_%02d.pcm", outDir, i+1)
		wavPath := fmt.Sprintf("%s/seg_%02d.wav", outDir, i+1)
		os.WriteFile(pcmPath, seg.Audio, 0644)
		writeWavFile(wavPath, seg.Audio, sampleRate, channels)
		fmt.Printf("[SAVE] %s / %s (%.2fs)\n",
			pcmPath, wavPath,
			float64(len(seg.Audio))/float64(sampleRate*2),
		)
	}

	fmt.Printf("\n✅ 共检测到 %d 段语音\n", i)
	fmt.Println("输出目录:", outDir)
	time.Sleep(100 * time.Millisecond)
}

// writeWavFile 写入带WAV头的音频文件
func writeWavFile(filename string, pcmData []byte, sampleRate, channels int) error {
	dataLen := len(pcmData)
	byteRate := sampleRate * channels * 2
	blockAlign := channels * 2
	fileSize := 36 + dataLen

	buf := new(bytes.Buffer)

	// 写入 RIFF Header
	buf.WriteString("RIFF")
	binary.Write(buf, binary.LittleEndian, uint32(fileSize))
	buf.WriteString("WAVE")

	// fmt chunk
	buf.WriteString("fmt ")
	binary.Write(buf, binary.LittleEndian, uint32(16))         // Subchunk1Size
	binary.Write(buf, binary.LittleEndian, uint16(1))          // AudioFormat = PCM
	binary.Write(buf, binary.LittleEndian, uint16(channels))   // NumChannels
	binary.Write(buf, binary.LittleEndian, uint32(sampleRate)) // SampleRate
	binary.Write(buf, binary.LittleEndian, uint32(byteRate))   // ByteRate
	binary.Write(buf, binary.LittleEndian, uint16(blockAlign)) // BlockAlign
	binary.Write(buf, binary.LittleEndian, uint16(16))         // BitsPerSample

	// data chunk
	buf.WriteString("data")
	binary.Write(buf, binary.LittleEndian, uint32(dataLen))
	buf.Write(pcmData)

	return os.WriteFile(filename, buf.Bytes(), 0644)
}
