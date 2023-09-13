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
//CTU의 크기
#define CTU_COUNT_ROW 8 //세로로 8개의 CTU가 생성됨
#define CTU_COUNT_COL 13 //가로로 13개의 CTU가 생성됨

enum //INTRA or INTER
{
	INTRA = 1, INTER = 2
};

enum //INTER의 모드
{
	AMVP = 1, MERGE = 2, SKIP = 3
};