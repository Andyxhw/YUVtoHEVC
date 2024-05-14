#include<stdio.h>
#include<libavcodec/avcodec.h>
#include<libavformat/avformat.h>
#include "libswscale/swscale.h"
#include<libavutil/imgutils.h>
#include <libavutil/time.h>
#include<libavutil/opt.h>

const char* inFileName = "./a.yuv";
const char* outFileName = "./test_1920x1080.h265";
 
 
/*int encode(AVCodecContext* codecContent, AVPacket* packet, AVFrame* frame, FILE* outFile)
{
	//编码
	int ret = avcodec_send_frame(codecContent, frame);
	if (ret < 0)
	{
		fprintf(stderr, "Error sending a frame for encoding\n");
		return -1;
	}
 
	while (ret == 0)
	{
 
		ret = avcodec_receive_packet(codecContent, packet);
 
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
			fprintf(stdout, "need more data.");
			return 0;
		}
		else if (ret < 0) {
			fprintf(stderr, "Error encoding video frame\n");
			return -1;
		}
 
		if (ret == 0)
		{
			fprintf(stdout, "try to write file.");
			fwrite(packet->data, 1, packet->size, outFile);
		}
 
	}
}*/
 
int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index)
{
	int ret;
	int got_frame;
	AVPacket enc_pkt;
	//av_bitstream_filter_init("hevc_mp4toannexb");

	if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities /*& CODEC_CAP_DELAY*/))
	{
		return 0;
	}

	while (1)
	{
		printf("Flushing stream #%u encoder.\n", stream_index);
		enc_pkt.data = NULL;
		enc_pkt.size = 0;
		av_init_packet(&enc_pkt);
		ret = avcodec_encode_video2(fmt_ctx->streams[stream_index]->codec, &enc_pkt,
			NULL, &got_frame);
		av_frame_free(NULL);
		if (ret < 0)
			break;
		if (!got_frame)
		{
			ret = 0;
			break;
		}
		printf("Flush Encoder: Succeed to encode 1 frame! size:%5d.\n", enc_pkt.size);
		// mux encoded frame
		ret = av_write_frame(fmt_ctx, &enc_pkt);
		if (ret < 0)
		{
			break;
		}
	}
	return ret;
}
 
int main(int argc, char* argv[])
{
 
	/*int ret = 0;
 
	AVCodec* codec = NULL;
	AVCodecContext* codecContent = NULL;
	AVPacket* packet = NULL;
	AVFrame* frame = NULL;
 
	FILE* inFile = NULL;
	FILE* outFile = NULL;
 
 printf("AVERROR(EAGAIN) is %d",AVERROR(EAGAIN));
	printf("AVERROR_EOF is %d",AVERROR_EOF);
 
	//查找指定编码器
	codec = avcodec_find_encoder(AV_CODEC_ID_HEVC);
	if (codec == NULL)
	{
		printf("could not find h264 encoder!");
		return -1;
	}
 
	//申请编码器上下文
	codecContent = avcodec_alloc_context3(codec);
	if (codecContent == NULL)
	{
		printf("could not alloc h264 content!");
		return -1;
	}
 
	//必设参数
	codecContent->width = 1920;
	codecContent->height = 1080;
	codecContent->time_base.num = 1;
	codecContent->time_base.den = 25;
 
 
	codecContent->pix_fmt = AV_PIX_FMT_YUV420P;
	codecContent->gop_size = 30;//关键帧间隔，默认250
	codecContent->framerate.num = 25;
	codecContent->framerate.den = 1;
 
	//初始化编码器上下文
	ret = avcodec_open2(codecContent, codec, NULL);
	if (ret < 0) {
		fprintf(stderr, "Could not open codec: %d\n", ret);
		exit(1);
	}
 
 
	packet = av_packet_alloc();
	if (packet == NULL)
	{
		printf("alloc packet error");
		return -1;
	}
 
	frame = av_frame_alloc();
	if (packet == NULL)
	{
		printf("alloc frame error");
		return -1;
	}
 
	//必设参数
	frame->width = codecContent->width;
	frame->height = codecContent->height;
	frame->format = codecContent->pix_fmt;
 
	//设置该参数将导致视频全是I帧，忽略gop_size
	//frame->pict_type = AV_PICTURE_TYPE_I;
 
 
	//申请视频数据存储空间
	ret = av_frame_get_buffer(frame, 0);
	if (ret)
	{
		printf("alloc frame buffer error!");
		return -1;
	}
 
	inFile = fopen(inFileName, "rb");
	if (inFile == NULL)
	{
		printf("error to open file: %s\n", inFileName);
		return -1;
	}
 
	outFile = fopen(outFileName, "wb");
	if (inFile == NULL)
	{
		printf("error to open file: %s\n", outFileName);
		return -1;
	}
 
	//帧数记录
	int framecount = 0;
	
	frame->pts = 0;
 
	int start_time = av_gettime() / 1000; //毫秒级
 

	while (!feof(inFile))
	{
		ret = av_frame_is_writable(frame);
		if (ret < 0)
		{
			ret = av_frame_make_writable(frame);
		}
 
 
		fread(frame->data[0], 1, frame->width * frame->height, inFile); //y
		fread(frame->data[1], 1, (frame->width * frame->height) / 4, inFile); //u
		fread(frame->data[2], 1, (frame->width * frame->height) / 4, inFile);  //v

		printf("encode frame num: %d\n", ++framecount);
 
 
		frame->pts += 1000 / (codecContent->time_base.den / codecContent->time_base.num);
		encode(codecContent, packet, frame, outFile);
 
	}
 
	encode(codecContent, packet, NULL, outFile);
	printf("encode time cost: %d ms\n ", av_gettime() / 1000 - start_time);
 
	av_packet_free(&packet);
	av_frame_free(&frame);
	avcodec_free_context(&codecContent);
	fclose(inFile);
	fclose(outFile);
 
 
	return 0;*/
AVFormatContext* pFormatCtx;
	AVOutputFormat* fmt;
	AVStream* video_stream;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;

	AVPacket pkt;
	uint8_t* picture_buf;
	AVFrame* pFrame;

	int ret;
	int size = 0;
	int y_size = 0;
	int framecnt = 1;

	// Input raw YUV data
	FILE *fp_in = fopen(inFileName, "rb");
	// FILE *in_file = fopen("src01_480x272.yuv", "rb");
	const int in_width = 1920, in_height = 1080; // Input data's width and height
	int framenum = 100; // Frames to encode

	// Output Filepath
	// const char* out_file = "ds.h264";
	const char* out_file = "c.hevc";
	// const char* out_file = "src01.h264";
	// const char* out_file = "src01.ts";

	av_register_all();

	// Method 1
	// pFormatCtx = avformat_alloc_context();
	// fmt = av_guess_format(NULL, out_file, NULL); // Guess Format
	// pFormatCtx->oformat = fmt;

	// Method 2
	avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
	fmt = pFormatCtx->oformat;

	// Open output URL
	ret = avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE);
	if (ret < 0)
	{
		// ����ļ���ʧ��
		printf("Can't open output file.\n");
		return -1;
	}

	video_stream = avformat_new_stream(pFormatCtx, 0);
	video_stream->time_base.num = 1;
	video_stream->time_base.den = 25;

	if (video_stream == NULL)
	{
		printf("Can't create video stream.\n");
		return -1;
	}

	// Param that must set
	pCodecCtx = video_stream->codec;
	// pCodecCtx->codec_id = AV_CODEC_ID_HEVC;
	pCodecCtx->codec_id = fmt->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->width = in_width;
	pCodecCtx->height = in_height;
	pCodecCtx->bit_rate = 400000;
	pCodecCtx->gop_size = 250;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;
	// H.264
	// pCodecCtx->me_range = 16;
	// pCodecCtx->max_qdiff = 4;
	// pCodecCtx->qcompress = 0.6;
	pCodecCtx->qmin = 10;
	pCodecCtx->qmax = 51;

	// Optional Param
	pCodecCtx->max_b_frames = 3;

	// Set Option
	AVDictionary *param = 0;
	// H.264
	if (pCodecCtx->codec_id == AV_CODEC_ID_H264)
	{
		av_dict_set(&param, "preset", "slow", 0);
		av_dict_set(&param, "tune", "zerolatency", 0);
		// av_dict_set(&param, "profile", "main", 0);
	}
	// H.265
	if (pCodecCtx->codec_id == AV_CODEC_ID_H265)
	{
		av_dict_set(&param, "x265-params", "qp=20", 0);
		av_dict_set(&param, "preset", "ultrafast", 0);
		av_dict_set(&param, "tune", "zero-latency", 0);
	}

	// Show some Information
	av_dump_format(pFormatCtx, 0, out_file, 1);

	pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if (!pCodec)
	{
		// û���ҵ����ʵı�����
		printf("Can't find encoder.\n");
		return -1;
	}

	ret = avcodec_open2(pCodecCtx, pCodec, &param);
	if (ret < 0)
	{
		// ��������ʧ��
		printf("Failed to open encoder.\n");
		return -1;
	}

	pFrame = av_frame_alloc();
	size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	picture_buf = (uint8_t *)av_malloc(size);
	avpicture_fill((AVPicture *)pFrame, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);


	pFrame->width = in_width;
	pFrame->height = in_height;
	pFrame->format = AV_PIX_FMT_YUV420P;
	// Write File Header
	avformat_write_header(pFormatCtx, NULL);

	y_size = pCodecCtx->width * pCodecCtx->height;
	av_new_packet(&pkt, 3 * y_size);

	for (int i = 0; i < framenum; i++)
	{
		// Read raw YUV data
		if (fread(picture_buf, 1, y_size * 3 / 2, fp_in) <= 0)
		{
			// �ļ���ȡ����
			printf("Failed to read raw YUV data.\n");
			break;
		}
		else if (feof(fp_in))
		{
			break;
		}

		pFrame->data[0] = picture_buf; // Y
		pFrame->data[1] = picture_buf + y_size; // U
		pFrame->data[2] = picture_buf + y_size * 5 / 4; // V

		// PTS
		pFrame->pts = i;
		// pFrame->pts = i*(video_stream->time_base.den) / ((video_stream->time_base.num) * 25);

		int got_picture = 0;
		// Encode
		ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
		if (ret < 0)
		{
			printf("Failed to encode! \n");
			return -1;
		}
		if (got_picture == 1)
		{
			printf("Succeed to encode frame: %5d\tsize:%5d.\n", framecnt, pkt.size);
			framecnt++;
			pkt.stream_index = video_stream->index;
			ret = av_write_frame(pFormatCtx, &pkt);
			av_free_packet(&pkt);
		}
	}

	// Flush Encoder
	ret = flush_encoder(pFormatCtx, 0);
	if (ret < 0)
	{
		printf("Flushing encoder failed.\n");
		return -1;
	}

	// Write file trailer
	av_write_trailer(pFormatCtx);

	// Clean
	if (video_stream)
	{
		avcodec_close(video_stream->codec);
		av_free(pFrame);
		av_free(picture_buf);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);

	fclose(fp_in);

	system("pause");
	return 0;
}