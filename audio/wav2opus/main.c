#include <opus/opus.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)

// WAV文件头结构
typedef struct {
  char chunkID[4];  // "RIFF"
  uint32_t chunkSize;
  char format[4];          // "WAVE"
  char subchunk1ID[4];     // "fmt "
  uint32_t subchunk1Size;  // PCM = 16
  uint16_t audioFormat;    // PCM = 1
  uint16_t numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
  char subchunk2ID[4];  // "data"
  uint32_t subchunk2Size;
} WAVHeader;

int main(int argc, char *argv[]) {
  int bitrate = 32000;  // 设置目标比特率 (32 kbps)
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <wav_file> <opus_file>\n", argv[0]);
    return 1;
  }
  FILE *wavFile = fopen(argv[1], "rb");
  FILE *opusFile = fopen(argv[2], "wb");
  if (!wavFile || !opusFile) {
    perror("文件打开失败");
    return 1;
  }

  // 读取WAV文件头
  WAVHeader header;
  fread(&header, sizeof(WAVHeader), 1, wavFile);
  if (strncmp(header.chunkID, "RIFF", 4) != 0 ||
      strncmp(header.format, "WAVE", 4) != 0) {
    printf("不是有效的WAV文件。\n");
    fclose(wavFile);
    fclose(opusFile);
    return 1;
  }

  // 配置Opus编码器
  int error;
  OpusEncoder *encoder = opus_encoder_create(
      header.sampleRate, header.numChannels, OPUS_APPLICATION_AUDIO, &error);
  if (error != OPUS_OK) {
    printf("Opus编码器初始化失败: %s\n", opus_strerror(error));
    fclose(wavFile);
    fclose(opusFile);
    return 1;
  }

  // 设置目标比特率
  error = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(bitrate));
  if (error != OPUS_OK) {
    printf("设置比特率失败: %s\n", opus_strerror(error));
    opus_encoder_destroy(encoder);
    return 1;
  }

  // 关闭VBR模式，启用CBR
  error = opus_encoder_ctl(encoder, OPUS_SET_VBR(0));  // 0 代表关闭VBR
  if (error != OPUS_OK) {
    printf("禁用VBR失败: %s\n", opus_strerror(error));
    opus_encoder_destroy(encoder);
    return 1;
  }

  // 设置帧大小为20ms
  int frame_size = header.sampleRate / 50;  // 20ms的采样点数量
  int16_t *pcm_buffer =
      (int16_t *)malloc(frame_size * header.numChannels * sizeof(int16_t));
  unsigned char opus_data[4000];  // 输出Opus数据缓冲区

  // 开始读取WAV数据并编码成Opus
  int read_samples;
  while ((read_samples = fread(pcm_buffer, sizeof(int16_t),
                               frame_size * header.numChannels, wavFile)) > 0) {
    // 将PCM数据编码为Opus数据
    int opus_len = opus_encode(encoder, pcm_buffer, frame_size, opus_data,
                               sizeof(opus_data));
    if (opus_len < 0) {
      printf("编码错误: %s\n", opus_strerror(opus_len));
      break;
    }

    // 将编码后的Opus数据写入输出文件
    fwrite(opus_data, 1, opus_len, opusFile);
  }

  // 释放资源
  free(pcm_buffer);
  opus_encoder_destroy(encoder);
  fclose(wavFile);
  fclose(opusFile);

  printf("WAV文件%s已成功转换为Opus文件%s\n", argv[1], argv[2]);
  return 0;
}
