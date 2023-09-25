#pragma once
#include "Macro.h"

//원본 영상 (프레임 1장)

class Input
{
public:
	unsigned char Y[Y_ROW][Y_COL];
	unsigned char Cb[CB_ROW][CB_COL];
	unsigned char Cr[CR_ROW][CR_COL];
};