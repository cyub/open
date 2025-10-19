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

	"github.com/baabaaox/go-webrtcvad"
)

// ======== 参数配置 ========
const (
	sampleRate   = 16000 // 采样率 8kHz
	frameMs      = 20    // 每帧 20ms
	frameSamples = sampleRate * frameMs / 1000
	frameBytes   = frameSamples * 2 // 16bit PCM，每个样本2字节
	frameSize    = sampleRate / 1000 * frameMs
	channels     = 1 // 单声道

	hangoverFrames  = 15 // 静音缓冲窗口（20ms * 15 = 300ms）
	minSpeechFrames = 10 // 最短语音段长度（20ms * 10 = 200ms）
)

// Segment 表示检测出的一个语音片段
type Segment struct {
	StartFrame int
	EndFrame   int
	Audio      []byte
}

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

	vadInst := webrtcvad.Create()
	defer webrtcvad.Free(vadInst)
	webrtcvad.Init(vadInst)
	err = webrtcvad.SetMode(vadInst, 3)
	if err != nil {
		log.Fatal(err)
	}
	var (
		frameBuf   = make([]byte, frameBytes)
		speechBuf  bytes.Buffer
		frameIndex int
		inSpeech   bool
		silenceCnt int
		startFrame int
		segments   []Segment
	)

	for {
		n, err := io.ReadFull(file, frameBuf)
		if err == io.EOF || err == io.ErrUnexpectedEOF {
			break
		}
		if err != nil {
			panic(err)
		}
		frameIndex++
		frame := frameBuf[:n]

		isSpeech, err := webrtcvad.Process(vadInst, sampleRate, frame, frameSize)
		if err != nil {
			panic(err)
		}

		if isSpeech {
			if !inSpeech {
				inSpeech = true
				startFrame = frameIndex
				speechBuf.Reset()
				fmt.Printf("[VAD] Speech start at frame %d\n", frameIndex)
			}
			silenceCnt = 0
			speechBuf.Write(frame)
		} else if inSpeech {
			silenceCnt++
			speechBuf.Write(frame)
			if silenceCnt > hangoverFrames {
				endFrame := frameIndex
				speechLen := speechBuf.Len()
				if speechLen >= frameBytes*minSpeechFrames {
					fmt.Printf("[VAD] Speech end at frame %d (%d bytes)\n", endFrame, speechLen)
					audioCopy := make([]byte, speechLen)
					copy(audioCopy, speechBuf.Bytes())
					segments = append(segments, Segment{
						StartFrame: startFrame,
						EndFrame:   endFrame,
						Audio:      audioCopy,
					})
				} else {
					fmt.Printf("[VAD] Discard short segment (%d bytes)\n", speechLen)
				}
				inSpeech = false
				silenceCnt = 0
			}
		}
	}

	// 保存分段结果
	for i, seg := range segments {
		pcmPath := fmt.Sprintf("%s/seg_%02d.pcm", outDir, i+1)
		wavPath := fmt.Sprintf("%s/seg_%02d.wav", outDir, i+1)
		os.WriteFile(pcmPath, seg.Audio, 0644)
		writeWavFile(wavPath, seg.Audio, sampleRate, channels)
		fmt.Printf("[SAVE] %s / %s (%.2fs)\n",
			pcmPath, wavPath,
			float64(len(seg.Audio))/float64(sampleRate*2),
		)
	}

	fmt.Printf("\n✅ 共检测到 %d 段语音\n", len(segments))
	fmt.Println("输出目录:", outDir)
	time.Sleep(100 * time.Millisecond)
}

// ======== WAV 写入函数 ========

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
