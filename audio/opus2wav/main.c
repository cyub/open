#include <opus/opus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CHANNELS 1          // 单声道
#define BITS_PER_SAMPLE 16  // 位深

#pragma pack(1)

// 定义WAV文件头结构
typedef struct {
  char chunkID[4];     // "RIFF"
  uint32_t chunkSize;  // 文件大小 - 8字节
  char format[4];      // "WAVE"

  char subchunk1ID[4];     // "fmt "
  uint32_t subchunk1Size;  // fmt子块大小，PCM为16
  uint16_t audioFormat;    // 音频格式，PCM为1
  uint16_t numChannels;    // 通道数
  uint32_t sampleRate;     // 采样率
  uint32_t byteRate;       // 每秒字节数
  uint16_t blockAlign;     // 每采样块的字节数
  uint16_t bitsPerSample;  // 每样本位数，即位深

  char subchunk2ID[4];     // "data"
  uint32_t subchunk2Size;  // 音频数据的大小
} WAVHeader;

void init_wav_header(WAVHeader *header, int sampleRate, int channels,
                     int bitsPerSample, int dataSize) {
  memcpy(header->chunkID, "RIFF", 4);
  header->chunkSize = dataSize + 36;
  memcpy(header->format, "WAVE", 4);
  memcpy(header->subchunk1ID, "fmt ", 4);
  header->subchunk1Size = 16;
  header->audioFormat = 1;
  header->numChannels = channels;
  header->sampleRate = sampleRate;
  header->byteRate = sampleRate * channels * bitsPerSample / 8;
  header->blockAlign = channels * bitsPerSample / 8;
  header->bitsPerSample = bitsPerSample;
  memcpy(header->subchunk2ID, "data", 4);
  header->subchunk2Size = dataSize;
}

int packet_size(int bit_rate, int duration) {
  return (bit_rate / (8 * 1000) * duration);
}

int frame_size(int sample_rate, int duration) {
  return (sample_rate / 1000 * duration);
}

typedef struct {
  int sample_rate;
  int bit_rate;
  int frame_duration;
  char *input;
  char *output;
} Config;

void usage(const char *prog_name) {
  fprintf(stderr, "Usage: %s [OPTIONS] input.opus output.wav\n\n", prog_name);
  fprintf(stderr, "Options:\n");
  fprintf(stderr,
          "  -r <sample_rate>      Sample rate in Hz (default: 16000)\n");
  fprintf(stderr,
          "  -b <bit_rate>         Bit rate in kbps (default: 32000)\n");
  fprintf(stderr,
          "  -d <frame_duration>   Frame duration in ms (default: 20)\n");
  fprintf(stderr, "\nArguments:\n");
  fprintf(stderr,
          "  input.opus            Input file (must be an opus file)\n");
  fprintf(stderr,
          "  output.wav            Output file (must be a wav file)\n\n");
  fprintf(stderr, "Example:\n");
  fprintf(stderr, "  %s -r 44100 -b 24 input.opus output.wav\n", prog_name);
  fprintf(stderr, "  %s -d 30 sample.opus out.wav\n", prog_name);
}

int parse_arguments(int argc, char *argv[], Config *config) {
  int opt;
  while ((opt = getopt(argc, argv, "r:b:d:h")) != -1) {
    switch (opt) {
      case 'r':
        config->sample_rate = atoi(optarg);
        if (config->sample_rate <= 0) {
          fprintf(stderr, "Invalid sample rate: %s\n", optarg);
          return -1;
        }
        break;
      case 'b':
        config->bit_rate = atoi(optarg);
        if (config->bit_rate <= 0) {
          fprintf(stderr, "Invalid bit rate: %s\n", optarg);
          return -1;
        }
        break;
      case 'd':
        config->frame_duration = atoi(optarg);
        if (config->frame_duration <= 0) {
          fprintf(stderr, "Invalid frame duration: %s\n", optarg);
          return -1;
        }
        break;
      case 'h':
        usage(argv[0]);
        exit(EXIT_SUCCESS);
      default:
        usage(argv[0]);
        return -1;
    }
  }

  if (optind + 2 != argc) {
    fprintf(stderr, "Error: Missing required input/output files\n\n");
    usage(argv[0]);
    return -1;
  }

  config->input = argv[optind];
  config->output = argv[optind + 1];

  return 0;
}

int raw_opus_to_wav(const char *opus_file, const char *wav_file,
                    int sample_rate, int bit_rate, int duration_ms) {
  FILE *opus_fp = fopen(opus_file, "rb");
  if (!opus_fp) {
    perror("open opus file");
    return -1;
  }

  FILE *wav_fp = fopen(wav_file, "wb");
  if (!wav_fp) {
    perror("open wav file");
    fclose(opus_fp);
    return -1;
  }

  // 预写WAV头部
  WAVHeader wavHeader;
  fwrite(&wavHeader, sizeof(WAVHeader), 1, wav_fp);
  // 创建Opus编码器
  int error;
  OpusDecoder *decoder = opus_decoder_create(sample_rate, CHANNELS, &error);
  if (error != OPUS_OK) {
    fprintf(stderr, "create opus decoder error: %s\n", opus_strerror(error));
    fclose(opus_fp);
    fclose(wav_fp);
    return -1;
  }

  int pack_size = packet_size(bit_rate, duration_ms);
  unsigned char packet[pack_size];
  int frsize = frame_size(sample_rate, duration_ms);
  short pcm[frsize * CHANNELS];
  int n;
  long int pcm_data_size = 0;

  while (1) {
    size_t packet_size = fread(packet, 1, pack_size, opus_fp);
    if (packet_size == 0) {
      break;
    }

    n = opus_decode(decoder, packet, packet_size, pcm, frsize, 0);
    if (n < 0) {
      fprintf(stderr, "decode error %s\n", opus_strerror(n));
      break;
    }
    pcm_data_size += n * CHANNELS + sizeof(short);
    fwrite(pcm, sizeof(short), n * CHANNELS, wav_fp);
  }

  // 更新WAV头部信息
  init_wav_header(&wavHeader, sample_rate, CHANNELS, BITS_PER_SAMPLE,
                  pcm_data_size);
  fseek(wav_fp, 0, SEEK_SET);
  fwrite(&wavHeader, sizeof(WAVHeader), 1, wav_fp);

  opus_decoder_destroy(decoder);
  fclose(opus_fp);
  fclose(wav_fp);
  return 0;
}

int main(int argc, char *argv[]) {
  Config config = {
      .sample_rate = 16000,
      .bit_rate = 32000,
      .frame_duration = 20,
  };

  if (parse_arguments(argc, argv, &config) != 0) {
    exit(EXIT_FAILURE);
  }

  if (config.sample_rate <= 0 || config.bit_rate <= 0 ||
      config.frame_duration <= 0) {
    fprintf(stderr, "invalid option");
    exit(1);
  }

  if (raw_opus_to_wav(config.input, config.output, config.sample_rate,
                      config.bit_rate, config.frame_duration) == 0) {
    printf("转换完成\n");
  } else {
    printf("转换失败\n");
  }
  return 0;
}