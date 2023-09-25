#pragma once

#include "Macro.h"

//Prediction (프레임 1장)
//Picture_YCbCr = Prediction + Residual

//Prediction.xml 파일을 생성하거나, 
//읽어서 예측 정보만으로 Prediction.yuv를 생성함 (디버그용) (잔차 정보는 제외되어 있음)

class Prediction
{
public:
	unsigned char Y[Y_ROW][Y_COL];
	unsigned char Cb[CB_ROW][CB_COL];
	unsigned char Cr[CR_ROW][CR_COL];
};