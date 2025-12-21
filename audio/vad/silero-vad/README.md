# Silero VAD

Silero VAD（语音活动检测）是 Silero AI 开发的一款轻量级、高性能的深度学习模型，专门用于快速准确地检测音频流中的语音片段，区分人声与背景噪音。它特点是体积小（仅2MB左右）、速度快、支持多语言、对噪声鲁棒性强，适用于语音助手、实时通信、转录服务等需要处理语音信号的场景。 

**主要特点：**

- 基于深度学习：利用神经网络技术，比传统方法（如WebRTC VAD）在复杂环境下检测更准确。
- 轻量高效：模型小，处理速度快，非常适合边缘计算和实时应用。
- 通用性强：在包含数千种语言的大型数据集上训练，对各种噪音和音频质量都表现良好。
- 灵活输入：支持任意采样率的PCM音频输入。
- 易于集成：提供Python API，可通过PyTorch和ONNX在多种平台上运行。 

**核心功能：**

- 语音段检测：输入音频，输出语音段的起止时间。
- 实时流处理：支持逐帧判断，用于实时语音交互。



## 安装方式

安装方式   |  命令   |  适用场景   |  优势
--- | --- | --- | ---
PyPI官方包   |  pip install silero-vad   |  快速验证、生产环境   |  自动依赖管理、版本控制
TorchHub加载   |  torch.hub.load('snakers4/silero-vad')   |  模型对比、学术研究   |  无需手动下载模型、多版本切换方便
源码编译   |  git clone https://github.com/snakers4/silero-vad.git   |  二次开发、定制优化| 	可修改源码、支持最新特性

## 模型选择

Silero VAD包含四种不同特性的模型版本：

模型文件  |  格式  |  特性  |  大小 | 推理速度 |  适用场景
--- | --- | --- | --- | --- | ---
silero_vad.jit  |  PyTorch JIT  |  完整精度，原生支持 | 2MB  |  最快 | Python后端应用
silero_vad.onnx  |  ONNX  |  跨平台兼容，OPSET 16  |  5MB| 快 | 多语言部署
silero_vad_16k_op15.onnx  |  ONNX  |  低版本OPSET兼容  | 5MB | 较快 | 旧版推理引擎
silero_vad_half.onnx  |  ONNX  |  半精度，体积更小  | 2.5MB | 较快 |  移动端/嵌入式

## 示例

### 最简单示例：音频文件分段处理

```python
from silero_vad import load_silero_vad, read_audio, get_speech_timestamps
 
# 加载模型（默认JIT格式）
model = load_silero_vad(onnx=False)  # onnx=True可加载ONNX模型
 
# 读取音频文件（自动处理格式转换）
audio = read_audio('test.wav', sampling_rate=16000)
 
# 获取语音时间戳
speech_timestamps = get_speech_timestamps(
    audio,
    model,
    threshold=0.5,  # 语音概率阈值
    min_speech_duration_ms=150,  # 最小语音片段长度，小于则不算作语音片段
    min_silence_duration_ms=200,  # 最小静音间隔
    return_seconds=True  # 返回秒级时间戳（默认采样点）
)
 
print("语音片段时间戳：", speech_timestamps)
# 输出示例：[{'start': 0.5, 'end': 2.3}, {'start': 3.1, 'end': 5.8}]
```

`get_speech_timestamps` 函数核心参数：

参数名  |  类型  |  默认值  |  作用范围  |  调优建议
--- | --- | --- | --- | ---
threshold  |  float  |  0.5  |  [0.1, 0.9]  |  噪音大场景提高至0.6-0.7，干净场景降低至0.3-0.4
min_speech_duration_ms  |  int  |  250  |  [50, 1000]  |  短句检测设为100-150，长句设为300-500
min_silence_duration_ms  |  int  |  100  |  [50, 500]  |  避免断句设为50-80，严格分段设为200-300
speech_pad_ms  |  int  |  30  |  [0, 100]  |  需要保留前后静音设为50-100
max_speech_duration_s  |  float  |  inf  |  [1, 300]  |  会议场景设为60-120，避免超长语音段


### 实时处理：实时麦克风识别

```python
import pyaudio
import numpy as np
from silero_vad import VADIterator
 
# 初始化VAD迭代器
model = load_silero_vad(onnx=True) # 加载onnx模型
vad_iterator = VADIterator(model, threshold=0.5)
 
# 配置音频流
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 16000
CHUNK = 512  # 32ms @ 16kHz
 
audio = pyaudio.PyAudio()
stream = audio.open(format=FORMAT, channels=CHANNELS,
                    rate=RATE, input=True,
                    frames_per_buffer=CHUNK)
 
print("开始录音... (按Ctrl+C停止)")
try:
    while True:
        data = stream.read(CHUNK)
        # 转换为模型输入格式
        audio_chunk = np.frombuffer(data, dtype=np.int16).astype(np.float32) / 32768.0
        # 处理音频块
        result = vad_iterator(audio_chunk, return_seconds=True)
        if result:
            if 'start' in result:
                print(f"语音开始: {result['start']:.2f}s")
            else:
                print(f"语音结束: {result['end']:.2f}s")
except KeyboardInterrupt:
    pass
finally:
    stream.stop_stream()
    stream.close()
    audio.terminate()
```

## 参考来源

- [最完整Silero VAD指南：从入门到生产环境部署](https://blog.csdn.net/gitblog_00942/article/details/151291883)
