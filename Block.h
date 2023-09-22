#pragma once

#include <vector>

#include "Macro.h"
using namespace std;

class Intra
{
public:
	unsigned char mode = 0; //1 ~ 35 (intra�� 1�� ������ 35�� ������)
	//0�� �� ���� �� (�ʱ�ȭ ���� ����)
};

class MV //Inter�� ����� ������ ����
{
public:
	
};

class Inter
{
public:
	unsigned char mode = 0; //1 ~ 3 (1 = AMVP, 2 = MERGE, 3 = SKIP)
	//0�� �� ���� �� (�ʱ�ȭ ���� ����)

	//         AMVP MERGE SKIP
	//MV       X    X     X (MP = *pMVP + MVD. ���� ����� �ƴ����� �� ���� ���ڴ������� ������ ������)
	//pMVP     O    O     O
	//MVD      O    X     X
	//residual O    O     X
};

//PU 1���� 1���� ��� (Intra��� 1 ~ 35, Inter��� 1 ~ 3)�� ������
class PU
{
public:
	bool split_tu_flag;
	unsigned char size; //�� PU�� ����&���� ���� (4, 8, 16, 32, 64)
	//false -> �� PU�� TU�� 1�� ����
	//true -> �� PU�� TU�� 4�� ����
	PU** PUs; //ũ�Ⱑ 1 �Ǵ� 4�� �迭
	//PU 1���� �ּ� �Ǵ� PU 4���� �ּ�

	Intra* p_Intra; //CU�� intra_or_inter�� INTRA�� ���� ���ǹ���
	Inter* p_Inter; //CU�� intra_or_inter�� INTER�� ���� ���ǹ���
};

//CU 1���� Intra or Inter�� ������
class CU
{
public:
	bool split_pu_flag;
	unsigned char size; //�� CU�� ����&���� ���� (8, 16, 32, 64)
	//false -> �� CU�� PU�� 1�� ����
	//true -> �� CU�� PU�� 4�� ����
    unsigned char intra_or_inter; //INTRA -> �� CU�� intra ������ ��. INTER -> �� CU�� inter ������ ��
	PU** PUs; //ũ�Ⱑ 1 �Ǵ� 4�� �迭
	//PU 1���� �ּ� �Ǵ� PU 4���� �ּ�
};

//QuadTreeNode 1���� CU 1���� �����ǰų�, QuadTreeNode 4���� ������
class QuadTreeNode
{
public:
	bool split_cu_flag; 
	//false -> CU 1���� ������
	//true -> QuadTreeNode 4���� ������
	unsigned char size; //�� ����� ����&���� ���� (8, 16, 32, 64) (depth�� ������ �� ����)

	QuadTreeNode** QuadTreeNodes; //split_cu_flag�� true�� ���� ���ǹ��� �ڷᱸ��
	//QuadTreeNode 4���� �ּ�

	CU* CUs; //split_cu_flag�� false�� ���� ���ǹ��� �ڷᱸ��
	//CU�� �ּ�
};

class QuadTree
{
public:
	QuadTreeNode* root;
};

//CTU
class CTU
{
public:
	unsigned short row_top_left; //(���� CTU�� ���� �ǿ��� �ȼ�)�� �� ��ȣ (0 ~ 479)
	unsigned short col_top_left; //(���� CTU�� ���� �ǿ��� �ȼ�)�� �� ��ȣ (0 ~ 831)
	QuadTree quadtree; //CTU 1���� 1���� ����Ʈ���� ����
};

//������ 1���� Picture 1���� ������
//�ϳ��� Picture �������� CTU�� 2���� vector�� ���� (Y, Cb, Cr ���� 1���� �� 3��)
//�ϳ��� CTU �������� CU�� Ʈ�� ������ ����
//DPB.h�� Picture_YCbCr���� �ٸ�
//YCbCr �ȼ��� �Ӹ� �ƴ϶� ��� ���� ������ ���Ե� 2���� �迭 3��
class Picture_Block
{
public:
	vector<vector<CTU>> CTUs_Y;
	vector<vector<CTU>> CTUs_Cb;
	vector<vector<CTU>> CTUs_Cr;

	Picture_Block() //CTUs 3���� ������
	{
		CTUs_Y.resize(CTU_Y_COUNT_ROW, vector<CTU>(CTU_Y_COUNT_COL)); //8 x 13
		CTUs_Cb.resize(CTU_Cb_COUNT_ROW, vector<CTU>(CTU_Cb_COUNT_COL)); //8 x 13
		CTUs_Cr.resize(CTU_Cr_COUNT_ROW, vector<CTU>(CTU_Cr_COUNT_COL)); //8 x 13

		for (unsigned int i = 0; i < CTU_Y_COUNT_ROW; i++)
			for (unsigned int j = 0; j < CTU_Y_COUNT_COL; j++)
			{
				CTUs_Y[i][j].row_top_left = i * CTU_Y_SIZE_ROW; //0, 64, 128, 192, 256, 320, 384, 448
				CTUs_Y[i][j].col_top_left = j * CTU_Y_SIZE_COL; //0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768
			}
		for (unsigned int i = 0; i < CTU_Cb_COUNT_ROW; i++)
			for (unsigned int j = 0; j < CTU_Cb_COUNT_COL; j++)
			{
				CTUs_Cb[i][j].row_top_left = i * CTU_Cb_SIZE_ROW; //0, 32, 64, 96, 128, 160, 192, 224
				CTUs_Cb[i][j].col_top_left = j * CTU_Cb_SIZE_COL; //0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384
			}
		for (unsigned int i = 0; i < CTU_Cr_COUNT_ROW; i++)
			for (unsigned int j = 0; j < CTU_Cr_COUNT_COL; j++)
			{
				CTUs_Cr[i][j].row_top_left = i * CTU_Cr_SIZE_ROW; //0, 32, 64, 96, 128, 160, 192, 224
				CTUs_Cr[i][j].col_top_left = j * CTU_Cr_SIZE_COL; //0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384
			}
		
		//Picture �� ��� CTU�� �ʱ�ȭ (�� ���� ���α׷� ���ۺ��� ������� �ٲ��� ����)
	}
};