#pragma once

#include "Macro.h"

//Prediction (������ 1��)
//Picture_YCbCr = Prediction + Residual

//Prediction.xml ������ �����ϰų�, 
//�о ���� ���������� Prediction.yuv�� ������ (����׿�) (���� ������ ���ܵǾ� ����)

class Prediction
{
public:
	unsigned char Y[Y_ROW][Y_COL];
	unsigned char Cb[CB_ROW][CB_COL];
	unsigned char Cr[CR_ROW][CR_COL];
};