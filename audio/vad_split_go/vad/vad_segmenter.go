package main

import (
	"bytes"
	"fmt"
	"io"

	"github.com/baabaaox/go-webrtcvad"
)

type vadSegmenter struct {
	conf    VadConfig
	vadInst webrtcvad.VadInst
	err     error
}

type VadConfig struct {
	SampleRate     int
	FrameDuration  int
	HangoverFrame  int
	MinSpeechFrame int
	Model          int
}

type Segment struct {
	StartTime int
	EndTime   int
	Audio     []byte
}

func NewVadSegmenter(conf VadConfig) (*vadSegmenter, error) {
	vadInst := webrtcvad.Create()
	webrtcvad.Init(vadInst)

	err := webrtcvad.SetMode(vadInst, conf.Model)
	if err != nil {
		return nil, err
	}

	return &vadSegmenter{
		conf:    conf,
		vadInst: vadInst,
	}, nil
}

func (v *vadSegmenter) Process(r io.Reader) chan Segment {
	result := make(chan Segment)
	go v.doProcess(r, result)
	return result
}

func (v *vadSegmenter) doProcess(r io.Reader, result chan Segment) {
	var (
		frameSamples = v.conf.SampleRate * v.conf.FrameDuration / 1000
		frameBytes   = frameSamples * 2
		frameSize    = v.conf.SampleRate / 1000 * v.conf.FrameDuration
		frameBuf     = make([]byte, frameBytes)
		speechBuf    bytes.Buffer
		frameIndex   int
		inSpeech     bool
		silenceCnt   int
		startFrame   int
	)
	for {
		n, err := io.ReadFull(r, frameBuf)
		if err == io.EOF || err == io.ErrUnexpectedEOF {
			close(result)
			break
		}
		if err != nil {
			v.err = err
			close(result)
			break
		}
		frameIndex++
		frame := frameBuf[:n]
		isSpeech, err := webrtcvad.Process(v.vadInst, v.conf.SampleRate, frame, frameSize)
		if err != nil {
			v.err = err
			close(result)
			break
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
			if silenceCnt > v.conf.HangoverFrame {
				endFrame := frameIndex
				speechLen := speechBuf.Len()
				if speechLen >= frameBytes*v.conf.MinSpeechFrame {
					fmt.Printf("[VAD] Speech end at frame %d (%d bytes)\n", endFrame, speechLen)
					audioCopy := make([]byte, speechLen)
					copy(audioCopy, speechBuf.Bytes())
					result <- Segment{
						StartTime: startFrame * v.conf.FrameDuration,
						EndTime:   endFrame * v.conf.FrameDuration,
						Audio:     audioCopy,
					}
				} else {
					fmt.Printf("[VAD] Discard short segment (%d bytes)\n", speechLen)
				}
				inSpeech = false
				silenceCnt = 0
			}
		}
	}
}

func (v *vadSegmenter) Close() error {
	webrtcvad.Free(v.vadInst)
	return nil
}
