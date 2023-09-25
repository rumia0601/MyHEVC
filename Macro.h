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

#define Y_ROW SourceHeight
#define Y_COL SourceWidth
//Y

#define CB_ROW (SourceHeight / 2)
#define CB_COL (SourceWidth / 2)
//Cb

#define CR_ROW (SourceHeight / 2)
#define CR_COL (SourceWidth / 2)
//Cr

//======== CTU / CU / PU / TU ===============
#define CTU_Y_SIZE_ROW 64
#define CTU_Y_SIZE_COL 64
#define CTU_Y_COUNT_ROW 8 //세로로 8개의 CTU가 생성됨 (7.5이므로 맨 아래 CTU는 절반만 데이터가 있음)
#define CTU_Y_COUNT_COL 13 //가로로 13개의 CTU가 생성됨
//CTU Y

#define CTU_Cb_SIZE_ROW 32
#define CTU_Cb_SIZE_COL 32
#define CTU_Cb_COUNT_ROW 8 //세로로 8개의 CTU가 생성됨 (7.5이므로 맨 아래 CTU는 절반만 데이터가 있음)
#define CTU_Cb_COUNT_COL 13 //가로로 13개의 CTU가 생성됨
//CTU Cb

#define CTU_Cr_SIZE_ROW 32
#define CTU_Cr_SIZE_COL 32
#define CTU_Cr_COUNT_ROW 8 //세로로 8개의 CTU가 생성됨 (7.5이므로 맨 아래 CTU는 절반만 데이터가 있음)
#define CTU_Cr_COUNT_COL 13 //가로로 13개의 CTU가 생성됨
//CTU Cr

#define DPB_CAPICITY 16 //DPB에는 16개의 Picture가 저장될 수 있음

enum //INTRA or INTER
{
	INTRA = 1, INTER = 2
};

enum //INTER의 모드
{
	AMVP = 1, MERGE = 2, SKIP = 3
};