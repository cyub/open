package main

import (
	"bytes"
	"encoding/binary"
	"errors"
	"io"
	"math"

	"github.com/cyub/silero-vad-go/speech"
)

type vadSegmenter struct {
	conf VadConfig
	err  error
	sd   *speech.Detector
	speech.DetectorConfig
}

type VadConfig struct {
	speech.DetectorConfig
}

type Segment struct {
	StartTime int
	EndTime   int
	Audio     []byte
}

func NewVadSegmenter(conf VadConfig) (*vadSegmenter, error) {

	sd, err := speech.NewDetector(speech.DetectorConfig{
		ModelPath:            conf.ModelPath,
		SampleRate:           conf.SampleRate,
		Threshold:            conf.Threshold,
		MinSilenceDurationMs: conf.MinSilenceDurationMs,
		SpeechPadMs:          conf.SpeechPadMs,
	})
	if err != nil {
		return nil, err
	}
	return &vadSegmenter{
		conf: conf,
		sd:   sd,
	}, nil
}

func (v *vadSegmenter) Process(r io.Reader) chan Segment {
	result := make(chan Segment)
	go v.doProcess(r, result)
	return result
}

func (v *vadSegmenter) doProcess(r io.Reader, result chan Segment) {
	var frameBytes = 512 * 2
	if v.conf.DetectorConfig.SampleRate == 8000 {
		frameBytes = 256 * 2
	}

	var (
		frameBuf      = make([]byte, frameBytes)
		speechBuf     bytes.Buffer
		speechStartAt int = -1
	)

	defer close(result)
	for {
		n, err := io.ReadFull(r, frameBuf)
		if err == io.EOF || err == io.ErrUnexpectedEOF {
			break
		}
		if err != nil {
			v.err = err
			break
		}
		frame := frameBuf[:n]
		input := PCM16LEToF32(frame)
		event, err := v.sd.DetectStreamFrame(input)
		if err != nil {
			v.err = err
			return
		}

		if event == nil {
			if speechStartAt == -1 {
				continue
			}
			speechBuf.Write(frame)
			continue
		}

		if event.IsStart {
			speechBuf.Reset()
			speechStartAt = int(event.SpeechEndAt * 1000)
			speechBuf.Write(frame)
		} else if event.IsEnd {
			speechBuf.Write(frame)
			audioCopy := make([]byte, speechBuf.Len())
			copy(audioCopy, speechBuf.Bytes())
			result <- Segment{
				StartTime: speechStartAt,
				EndTime:   int(event.SpeechEndAt * 1000),
				Audio:     audioCopy,
			}
			speechStartAt = 0
		} else {
			v.err = errors.New("never reach here")
			return
		}
	}
}

func (v *vadSegmenter) Close() error {
	return v.sd.Destroy()
}

func (v *vadSegmenter) Error() error {
	return v.err
}

func PCM16LEToF32(src []byte) []float32 {
	if len(src)%2 != 0 {
		panic("odd length")
	}

	out := make([]float32, len(src)/2)
	for i := 0; i < len(out); i++ {
		val := int16(binary.LittleEndian.Uint16(src[2*i:]))
		// 归一化到[-1.0, 1.0)
		out[i] = float32(val) / float32(math.MaxInt16+1)
	}

	return out
}
