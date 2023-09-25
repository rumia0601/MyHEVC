#pragma once
#include <vector>

//프레임 1장
//Block.h의 Picture와는 다름
//블록 분할 정보가 없는, 순수 YCbCr 픽셀값들의 2차원 배열 3개
//Picture_YCbCr = Prediction + Residual
class Picture_YCbCr
{
public:
	unsigned char Y[Y_ROW][Y_COL];
	unsigned char Cb[CB_ROW][CB_COL];
	unsigned char Cr[CR_ROW][CR_COL];
};

//프레임 1장 ~ 16장
class RPL //Reference Picture List
{
public:
	vector<Picture_YCbCr> Pictures_YCbCr; //Picture_YCbCr가 최대 16개까지 들어갈 수 있는 벡터
};

class DPB //Decoded Picture Buffer
{
public:
	RPL RPL;
	//컨트롤러 로직 (ex : 범핑 등)은 구현하지 않음
};