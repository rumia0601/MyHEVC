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
#define CTU_Y_COUNT_ROW 8 //���η� 8���� CTU�� ������ (7.5�̹Ƿ� �� �Ʒ� CTU�� ���ݸ� �����Ͱ� ����)
#define CTU_Y_COUNT_COL 13 //���η� 13���� CTU�� ������
//CTU Y

#define CTU_Cb_SIZE_ROW 32
#define CTU_Cb_SIZE_COL 32
#define CTU_Cb_COUNT_ROW 8 //���η� 8���� CTU�� ������ (7.5�̹Ƿ� �� �Ʒ� CTU�� ���ݸ� �����Ͱ� ����)
#define CTU_Cb_COUNT_COL 13 //���η� 13���� CTU�� ������
//CTU Cb

#define CTU_Cr_SIZE_ROW 32
#define CTU_Cr_SIZE_COL 32
#define CTU_Cr_COUNT_ROW 8 //���η� 8���� CTU�� ������ (7.5�̹Ƿ� �� �Ʒ� CTU�� ���ݸ� �����Ͱ� ����)
#define CTU_Cr_COUNT_COL 13 //���η� 13���� CTU�� ������
//CTU Cr

#define DPB_CAPICITY 16 //DPB���� 16���� Picture�� ����� �� ����

enum //INTRA or INTER
{
	INTRA = 1, INTER = 2
};

enum //INTER�� ���
{
	AMVP = 1, MERGE = 2, SKIP = 3
};