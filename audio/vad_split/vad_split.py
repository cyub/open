# vad_split.py
import os
import argparse
import webrtcvad
from pydub import AudioSegment
from pyannote.audio import Pipeline

def convert_to_pcm(audio_path, sample_rate=16000):
    audio = AudioSegment.from_file(audio_path)
    audio = audio.set_frame_rate(sample_rate).set_channels(1).set_sample_width(2)
    return audio

def frame_generator(audio, frame_duration_ms, sample_rate=16000):
    n = int(sample_rate * (frame_duration_ms / 1000.0)) * 2
    offset = 0
    while offset + n < len(audio.raw_data):
        yield audio.raw_data[offset:offset + n]
        offset += n

def is_loud_enough(segment: AudioSegment, threshold_db: float):
    return segment.dBFS >= threshold_db

def vad_split_webrtc(audio_path, out_dir, aggressiveness, frame_duration_ms, sample_rate, silence_duration_ms, min_duration_sec, volume_threshold_db):
    vad = webrtcvad.Vad(aggressiveness)
    audio = convert_to_pcm(audio_path, sample_rate)
    frames = list(frame_generator(audio, frame_duration_ms, sample_rate))

    in_speech = False
    segment_start = 0
    silence_frames = 0
    silence_threshold = silence_duration_ms // frame_duration_ms
    voiced_segments = []

    for i, frame in enumerate(frames):
        is_speech = vad.is_speech(frame, sample_rate)
        if is_speech:
            if not in_speech:
                segment_start = i
                in_speech = True
            silence_frames = 0
        else:
            if in_speech:
                silence_frames += 1
                if silence_frames >= silence_threshold:
                    segment_end = i - silence_frames + 1
                    in_speech = False
                    voiced_segments.append((segment_start, segment_end))
                    silence_frames = 0
    if in_speech:
        voiced_segments.append((segment_start, len(frames)))

    os.makedirs(out_dir, exist_ok=True)
    for idx, (start, end) in enumerate(voiced_segments):
        start_ms = int(start * frame_duration_ms)
        end_ms = int(end * frame_duration_ms)
        duration_sec = (end_ms - start_ms) / 1000.0
        if duration_sec < min_duration_sec:
            continue
        segment = audio[start_ms:end_ms]
        if not is_loud_enough(segment, volume_threshold_db):
            continue
        out_path = os.path.join(out_dir, f"segment_{idx:03d}.wav")
        segment.export(out_path, format="wav")
        print(f"[+] Saved (VAD): {out_path}")

def vad_split_pyannote(audio_path, out_dir, min_duration_sec, volume_threshold_db):
    print("[*] Loading pyannote-audio pipeline...")
    pipeline = Pipeline.from_pretrained("pyannote/voice-activity-detection")
    vad_result = pipeline(audio_path)
    audio = AudioSegment.from_file(audio_path)
    os.makedirs(out_dir, exist_ok=True)

    for idx, segment in enumerate(vad_result.itertracks(yield_label=False)):
        start_ms = int(segment.start * 1000)
        end_ms = int(segment.end * 1000)
        duration_sec = (end_ms - start_ms) / 1000.0
        if duration_sec < min_duration_sec:
            continue
        chunk = audio[start_ms:end_ms]
        if not is_loud_enough(chunk, volume_threshold_db):
            continue
        out_path = os.path.join(out_dir, f"segment_{idx:03d}.wav")
        chunk.export(out_path, format="wav")
        print(f"[+] Saved (Pyannote): {out_path}")

def main():
    parser = argparse.ArgumentParser(description="Split audio using VAD (WebRTC or pyannote-audio)")
    parser.add_argument("input", help="Input .wav file")
    parser.add_argument("output", help="Output directory")
    
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--use-webrtc", action="store_true", help="Use WebRTC VAD")
    group.add_argument("--use-pyannote", action="store_true", help="Use pyannote-audio VAD")
    
    # WebRTC VAD params
    parser.add_argument("--mode", type=int, default=3, help="WebRTC VAD aggressiveness (0-3)")
    parser.add_argument("--frame", type=int, default=30, help="WebRTC frame duration in ms (10/20/30)")
    parser.add_argument("--silence", type=int, default=300, help="Silence duration (ms) to end segment")

    # Universal filters
    parser.add_argument("--min-duration", type=float, default=0.5, help="Minimum speech duration in seconds")
    parser.add_argument("--volume-threshold", type=float, default=-40.0, help="Minimum dBFS volume to keep segment")

    args = parser.parse_args()

    if args.use_webrtc:
        vad_split_webrtc(
            args.input, args.output,
            aggressiveness=args.mode,
            frame_duration_ms=args.frame,
            sample_rate=16000,
            silence_duration_ms=args.silence,
            min_duration_sec=args.min_duration,
            volume_threshold_db=args.volume_threshold
        )
    elif args.use_pyannote:
        vad_split_pyannote(
            args.input, args.output,
            min_duration_sec=args.min_duration,
            volume_threshold_db=args.volume_threshold
        )

if __name__ == "__main__":
    main()