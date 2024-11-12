#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <vorbis/vorbisenc.h>

#define READ_BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <input.wav> <output.ogg>\n", argv[0]);
    return 1;
  }

  const char *input_filename = argv[1];
  const char *output_filename = argv[2];

  // 打开输入WAV文件
  SF_INFO sfinfo;
  SNDFILE *infile = sf_open(input_filename, SFM_READ, &sfinfo);
  if (!infile) {
    fprintf(stderr, "无法打开输入文件: %s\n", sf_strerror(NULL));
    return 1;
  }

  // 检查输入文件是否为PCM格式
  if (sfinfo.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
    fprintf(stderr, "输入文件必须是16位PCM格式的WAV文件\n");
    sf_close(infile);
    return 1;
  }

  // 设置Vorbis编码器
  ogg_stream_state os;
  vorbis_info vi;
  vorbis_comment vc;
  vorbis_dsp_state vd;
  vorbis_block vb;

  vorbis_info_init(&vi);
  if (vorbis_encode_init_vbr(&vi, sfinfo.channels, sfinfo.samplerate, 0.5) !=
      0) {
    fprintf(stderr, "无法初始化Vorbis编码器\n");
    return 1;
  }

  vorbis_comment_init(&vc);
  vorbis_comment_add_tag(&vc, "ENCODER", "libvorbis");

  vorbis_analysis_init(&vd, &vi);
  vorbis_block_init(&vd, &vb);

  // 初始化Ogg流
  ogg_stream_init(&os, rand());

  // 写入Vorbis头部信息
  ogg_packet header;
  ogg_packet header_comm;
  ogg_packet header_code;

  vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
  ogg_stream_packetin(&os, &header);
  ogg_stream_packetin(&os, &header_comm);
  ogg_stream_packetin(&os, &header_code);

  FILE *outfile = fopen(output_filename, "wb");
  if (!outfile) {
    fprintf(stderr, "无法创建输出文件\n");
    return 1;
  }

  ogg_page og;
  while (ogg_stream_flush(&os, &og)) {
    fwrite(og.header, 1, og.header_len, outfile);
    fwrite(og.body, 1, og.body_len, outfile);
  }

  // 开始读取WAV文件并进行编码
  int16_t buffer[READ_BUFFER_SIZE];
  while (1) {
    int samples_read = sf_read_short(infile, buffer, READ_BUFFER_SIZE);
    if (samples_read == 0) break;

    float **vorbis_buffer =
        vorbis_analysis_buffer(&vd, samples_read / sfinfo.channels);
    for (int i = 0; i < samples_read / sfinfo.channels; i++) {
      for (int ch = 0; ch < sfinfo.channels; ch++) {
        vorbis_buffer[ch][i] = buffer[i * sfinfo.channels + ch] / 32768.f;
      }
    }

    vorbis_analysis_wrote(&vd, samples_read / sfinfo.channels);

    while (vorbis_analysis_blockout(&vd, &vb) == 1) {
      vorbis_analysis(&vb, NULL);
      vorbis_bitrate_addblock(&vb);

      ogg_packet op;
      while (vorbis_bitrate_flushpacket(&vd, &op)) {
        ogg_stream_packetin(&os, &op);

        while (ogg_stream_pageout(&os, &og)) {
          fwrite(og.header, 1, og.header_len, outfile);
          fwrite(og.body, 1, og.body_len, outfile);
        }
      }
    }
  }

  // 标记文件结束
  vorbis_analysis_wrote(&vd, 0);
  while (vorbis_analysis_blockout(&vd, &vb) == 1) {
    vorbis_analysis(&vb, NULL);
    vorbis_bitrate_addblock(&vb);

    ogg_packet op;
    while (vorbis_bitrate_flushpacket(&vd, &op)) {
      ogg_stream_packetin(&os, &op);

      while (ogg_stream_pageout(&os, &og)) {
        fwrite(og.header, 1, og.header_len, outfile);
        fwrite(og.body, 1, og.body_len, outfile);
      }
    }
  }

  // 清理资源
  ogg_stream_clear(&os);
  vorbis_block_clear(&vb);
  vorbis_dsp_clear(&vd);
  vorbis_comment_clear(&vc);
  vorbis_info_clear(&vi);

  sf_close(infile);
  fclose(outfile);

  printf("WAV文件已成功转换为Ogg格式\n");
  return 0;
}
