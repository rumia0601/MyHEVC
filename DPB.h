#pragma once
#include <vector>

//������ 1��
//Block.h�� Picture�ʹ� �ٸ�
//��� ���� ������ ����, ���� YCbCr �ȼ������� 2���� �迭 3��
//Picture_YCbCr = Prediction + Residual
class Picture_YCbCr
{
public:
	unsigned char Y[Y_ROW][Y_COL];
	unsigned char Cb[CB_ROW][CB_COL];
	unsigned char Cr[CR_ROW][CR_COL];
};

//������ 1�� ~ 16��
class RPL //Reference Picture List
{
public:
	vector<Picture_YCbCr> Pictures_YCbCr; //Picture_YCbCr�� �ִ� 16������ �� �� �ִ� ����
};

class DPB //Decoded Picture Buffer
{
public:
	RPL RPL;
	//��Ʈ�ѷ� ���� (ex : ���� ��)�� �������� ����
};