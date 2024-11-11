#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#pragma pack(1)  // 使结构体按1字节对齐

// 定义WAV文件头结构
typedef struct {
  char chunkID[4];     // "RIFF"
  uint32_t chunkSize;  // 文件大小 - 8
  char format[4];      // "WAVE"

  char subchunk1ID[4];     // "fmt "
  uint32_t subchunk1Size;  // fmt子块大小，PCM为16
  uint16_t audioFormat;    // 音频格式，PCM为1
  uint16_t numChannels;    // 通道数
  uint32_t sampleRate;     // 采样率
  uint32_t byteRate;       // 字节率
  uint16_t blockAlign;     // 块对齐
  uint16_t bitsPerSample;  // 位深度

  char subchunk2ID[4];     // "data"
  uint32_t subchunk2Size;  // 音频数据的大小
} WAVHeader;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <wav_file>\n", argv[0]);
    return 1;
  }
  FILE *file = fopen(argv[1], "rb");  // 打开WAV文件
  if (file == NULL) {
    perror("无法打开文件");
    return 1;
  }

  WAVHeader header;
  fread(&header, sizeof(WAVHeader), 1, file);  // 读取WAV头部

  // 打印WAV文件头信息
  printf("RIFF文件头标识: %.4s\n", header.chunkID);
  printf("文件格式: %.4s\n", header.format);
  printf("音频格式：");
  if (header.audioFormat == 1) {
    printf("PCM\n");
  } else if (header.audioFormat == 3) {
    printf("IEEE float\n");
  } else if (header.audioFormat == 6) {
    printf("A-law\n");
  } else if (header.audioFormat == 7) {
    printf("μ-law\n");
  } else if (header.audioFormat == 65534) {
    printf("可扩展PCM\n");
  }

  printf("通道数: %u\n", header.numChannels);
  printf("采样率: %u\n", header.sampleRate);
  assert(header.byteRate ==
         header.sampleRate * header.numChannels * header.bitsPerSample / 8);
  printf("每秒的数据字节数: %u\n", header.byteRate);

  printf("位深: %u\n", header.bitsPerSample);
  printf("音频数据的大小: %u\n", header.subchunk2Size);
  int numSamples =
      header.subchunk2Size / (header.numChannels * header.bitsPerSample / 8);
  printf("音频数据的采样次数：%d\n", numSamples);
  double duration = (double)header.subchunk2Size / header.byteRate;
  printf("音频时长: %.2f 秒\n", duration);
  fclose(file);  // 关闭文件
  return 0;
}