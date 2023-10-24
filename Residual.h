#pragma once

#include "Macro.h"

//Residual (프레임 1장)
//Picture_YCbCr = Prediction + Residual

//Residual.yuv를 생성함

class Residual
{
public:
	unsigned char Y[Y_ROW][Y_COL];
	unsigned char Cb[CB_ROW][CB_COL];
	unsigned char Cr[CR_ROW][CR_COL];
};