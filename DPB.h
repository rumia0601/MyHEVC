#include <vector>

#pragma once

//Block.h의 Picture와는 다름
//블록 분할 정보가 없는, 순수 YCbCr 픽셀값들의 2차원 배열 3개
class Picture_YCbCr
{
public:
	
};

class RPL //Reference Picture List
{
public:
	vector<Picture_YCbCr> Pictures_YCbCr;
};

class DPB //Decoded Picture Buffer
{
public:
	
};