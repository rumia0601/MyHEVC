#pragma once

#include "Macro.h"

//Residual (������ 1��)
//Picture_YCbCr = Prediction + Residual

//Residual.yuv�� ������

class Residual
{
public:
	unsigned char Y[Y_ROW][Y_COL];
	unsigned char Cb[CB_ROW][CB_COL];
	unsigned char Cr[CR_ROW][CR_COL];
};