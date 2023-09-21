#pragma once

#include <vector>

#include "Macro.h"
using namespace std;

class Intra
{
public:
	unsigned char mode = 0; //1 ~ 35 (intra의 1번 모드부터 35번 모드까지)
	//0은 안 쓰는 값 (초기화 안한 상태)
};

class Inter
{
public:
	unsigned char mode = 0; //1 ~ 3 (1 = AMVP, 2 = MERGE, 3 = SKIP)
	//0은 안 쓰는 값 (초기화 안한 상태)
};

//PU 1개는 1개의 모드 (Intra라면 1 ~ 35, Inter라면 1 ~ 3)를 결정함
class PU
{
	;
};

//CU 1개는 Intra or Inter를 결정함
class CU
{
public:
	bool split_cu_flag; //false -> 자식이 없음. true -> 자식이 4개 있음
    unsigned char intra_or_inter; //INTRA -> 이 CU는 intra 예측을 함. INTER -> 이 CU는 inter 예측을 함
	void* p_pu; //CU = PU이므로, QuadTreeNode 1개는 Intra 1개 또는 Inter 1개에만 대응됨
	// 해당 영역의 값 또는 데이터
};

//QuadTreeNode 1개는 CU 1개에 대응되거나, QuadTreeNode 4개에 대응됨
class QuadTreeNode
{
public:
	bool split_cu_flag; 
	//false -> CU 1개에 대응됨
	//true -> QuadTreeNode 4개에 대응됨
	unsigned char size; //이 노드의 가로&세로 길이 (8, 16, 32, 64) (depth를 유추할 수 있음)

	QuadTreeNode** QuadTreeNodes; //split_cu_flag가 true일 때만 유의미한 자료구조
	//QuadTreeNode 4개

	CU* p_CU; //split_cu_flag가 false일 때만 유의미한 자료구조
	//CU 1개
};

class QuadTree
{
public:
	
};

//CTU
class CTU
{
public:
	unsigned short row_top_left; //(현재 CTU의 맨위 맨왼쪽 픽셀)의 행 번호 (0 ~ 479)
	unsigned short col_top_left; //(현재 CTU의 맨위 맨왼쪽 픽셀)의 열 번호 (0 ~ 831)
	QuadTree quadtree; //CTU 1개는 1개의 쿼드트리를 가짐
};

//프레임 1개당 Picture 1개가 생성됨
//하나의 Picture 내에서는 CTU가 2차원 vector로 존재 (Y, Cb, Cr 별로 1개씩 총 3개)
//하나의 CTU 내에서는 CU가 트리 구조로 존재
//DPB.h의 Picture_YCbCr과는 다름
//YCbCr 픽셀값 뿐만 아니라 블록 분할 정보도 포함된 2차원 배열 3개
class Picture_Block
{
public:
	vector<vector<CTU>> CTUs_Y;
	vector<vector<CTU>> CTUs_Cb;
	vector<vector<CTU>> CTUs_Cr;

	Picture_Block() //CTUs 3개가 생성됨
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
		
		//Picture 내 모든 CTU를 초기화 (이 값은 프로그램 시작부터 종료까지 바뀌지 않음)
	}
};