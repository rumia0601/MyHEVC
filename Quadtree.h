#pragma once

enum
{
	INTRA = 1, INTER = 2
};

enum
{
	AMVP = 1, MERGE = 2, SKIP = 3
};

class Intra
{
public:
	Intra();
	~Intra();
	unsigned char mode; //1 ~ 35 (intra의 1번 모드부터 35번 모드까지)

};

class Inter
{
public:
	Inter();
	~Inter();
	unsigned char mode; //1 ~ 3 (1 = AMVP, 2 = MERGE, 3 = SKIP)
	
};

//노드 = CU = PU = TU
class QuadTreeNode
{
public:
	QuadTreeNode();
	~QuadTreeNode();

	bool split_cu_flag; //false -> 자식이 없음. true -> 자식이 4개 있음
    unsigned char intra_or_inter; //INTRA -> 이 CU는 intra 예측을 함. INTER -> 이 CU는 inter 예측을 함
	void* p_pu; //CU = PU이므로, QuadTreeNode 1개는 Intra 1개 또는 Inter 1개에만 대응됨
    QuadTreeNode* children[4]; // 네 개의 자식 노드
	// 해당 영역의 값 또는 데이터


};

class Quadtree
{
public:
	Quadtree();
	~Quadtree();

	
};