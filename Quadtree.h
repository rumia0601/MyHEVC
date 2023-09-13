#pragma once

#include "Macro.h"

class Intra
{
public:
	Intra();
	~Intra();
	unsigned char mode; //1 ~ 35 (intra�� 1�� ������ 35�� ������)
};

class Inter
{
public:
	Inter();
	~Inter();
	unsigned char mode; //1 ~ 3 (1 = AMVP, 2 = MERGE, 3 = SKIP)
};

//��� = CU = PU = TU
class QuadTreeNode
{
public:
	QuadTreeNode();
	~QuadTreeNode();

	bool split_cu_flag; //false -> �ڽ��� ����. true -> �ڽ��� 4�� ����
    unsigned char intra_or_inter; //INTRA -> �� CU�� intra ������ ��. INTER -> �� CU�� inter ������ ��
	void* p_pu; //CU = PU�̹Ƿ�, QuadTreeNode 1���� Intra 1�� �Ǵ� Inter 1������ ������
    QuadTreeNode* children[4]; // �� ���� �ڽ� ���
	// �ش� ������ �� �Ǵ� ������
};

//������ 1���� ����Ʈ�� 3�� (Y, U, V)�� ������
class Quadtree
{
public:
	Quadtree();
	~Quadtree();
};