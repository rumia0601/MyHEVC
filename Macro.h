#pragma once

//======== File I/O ===============
#define InputFile "BasketballDrill.yuv"
#define InputBitDepth 8 //Input bitdepth
#define InputChromaFormat 420 //Ratio of luminance to chrominance samples
#define FrameRate 50 //Frame Rate per second
#define FrameSkip 0 //Number of frames to be skipped in input
#define SourceWidth 832 //Input frame width
#define SourceHeight 480 //Input frame height
#define FramesToBeEncoded 500 //Number of frames to be coded
#define Level 3.1

//======== CTU / CU / PU / TU ===============
#define CTU_SIZE_ROW 64
#define CTU_SIZE_COL 64
//CTU�� ũ��
#define CTU_COUNT_ROW 8 //���η� 8���� CTU�� ������
#define CTU_COUNT_COL 13 //���η� 13���� CTU�� ������

enum //INTRA or INTER
{
	INTRA = 1, INTER = 2
};

enum //INTER�� ���
{
	AMVP = 1, MERGE = 2, SKIP = 3
};