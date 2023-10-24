#include <iostream>
#include <string>
#include <fstream>
#include <stack>
using namespace std;

#define CTU_SIZE 64
#define MAX_CU_SIZE 64
#define MIN_CU_SIZE 8
#define MIN_DEPTH 0
#define MAX_DEPTH 3

class CU
{
public:

};

class QUADTREENODE
{
public:
	int depth = 0;
	//루트 노드인 경우, 0이다
	//루트 노드가 아닌 경우, 1, 2 ... MAX_DEPTH 이다

	bool has_child = false;
	QUADTREENODE* top_left = NULL, * top_right = NULL, * bottom_left = NULL, * bottom_right = NULL;
	//단말 노드인 경우, NULL이다 (has_child = true)
	//단말 노드가 아닌 경우, 4개의 QUADTREE를 가르킨다 (has_child = false)

	CU* p_cu = NULL;
	//단말 노드는 1개의 CU를 가르킨다
	//단말 노드가 아닌 경우, NULL이다

	bool had_RDO = false;
	//RDO를 시도한 적이 있다면, true가 됨

	string pattern = "";
	//루트 노드인 경우, ""
	//루트 노드가 아닌 경우, "q", "qw", "qwa" 등으로 존재

	QUADTREENODE() //루트 노드일 때의 생성자
	{
		;
	}

	QUADTREENODE(int depth, string pattern) //루트 노드가 아닐 때의 생성자
	{
		this->depth = depth;
		this->pattern = pattern;
	}

	//소멸자 필요 없음 (자식의 자식은 없음)

	double RDOwithoutSplit()
	{
		double result = 0;
		return result;
	}

	double RDOwithSplit() //단말 노드가 아닌 경우, 자식 4개를 만듦. 그리고 RDO 점수를 냄
	{
		if (has_child == false)
			has_child = true;

		else if (has_child == true)
			exit(-2); //이미 나눴는데 또 나눌 수는 없음

		top_left = new QUADTREENODE(depth + 1, pattern + 'q');
		top_right = new QUADTREENODE(depth + 1, pattern + 'w');
		bottom_left = new QUADTREENODE(depth + 1, pattern + 'a');
		bottom_right = new QUADTREENODE(depth + 1, pattern + 's');

		double result1 = top_left->RDOwithoutSplit();
		double result2 = top_right->RDOwithoutSplit();
		double result3 = bottom_left->RDOwithoutSplit();
		double result4 = bottom_right->RDOwithoutSplit();
		double result = result1 + result2 + result3 + result4;

		return result;
	}

	void Split_undo() //만든 자식 4개를 다시 삭제 (롤백)
	{
		if (has_child == true)
			has_child = false;

		else if (has_child == false)
			exit(-3); //RDO를 해봤는데 자식이 없을 수는 없음

		if (had_RDO == false)
			had_RDO = true;

		else if (had_RDO == true)
			exit(-4); //RDO를 안 해봤는데 undo를 할 수는 없음

		delete top_left;
		delete top_right;
		delete bottom_left;
		delete bottom_right;

		top_left = NULL;
		top_right = NULL;
		bottom_left = NULL;
		bottom_right = NULL;
	}

	void print_pattern(ofstream& fout)
	{
		if (top_left != NULL)
			top_left->print_pattern(fout);
		if (top_right != NULL)
			top_right->print_pattern(fout);
		if (bottom_left != NULL)
			bottom_left->print_pattern(fout);
		if (bottom_right != NULL)
			bottom_right->print_pattern(fout);

		if (has_child == false)
			fout << pattern << endl;
	}
};

class QUADTREE
{
public:
	QUADTREENODE *root = NULL;

	QUADTREE() //생성자
	{
		root = new QUADTREENODE();
	}

	QUADTREENODE* FindCandidate(QUADTREENODE* current) //depth가 MAX_DEPTH가 아니고 had_RDO가 false인 QUADTREENODE들 중, DFS 했을 때 가장 먼저 만나는 QUADTRENODE
	{
		if (current->depth != MAX_DEPTH && current->has_child == false && current->had_RDO == false)
			return current;

		else if (current->depth != MAX_DEPTH && current->has_child == false && current->had_RDO == true)
			return NULL;

		else if (current->depth != MAX_DEPTH && current->has_child == true)
		{
			QUADTREENODE* result1 = FindCandidate(current->top_left); //왼쪽 위를 더 나눌 수 있는 지 확인
			if (result1 != NULL) //더 나눌 수 있음
				return result1; //왼쪽 위를 반환

			QUADTREENODE* result2 = FindCandidate(current->top_right);
			if (result2 != NULL)
				return result2;

			QUADTREENODE* result3 = FindCandidate(current->bottom_left);
			if (result3 != NULL)
				return result3;

			QUADTREENODE* result4 = FindCandidate(current->bottom_right);
			if (result4 != NULL)
				return result4;
		}

		else if (current->depth == MAX_DEPTH && current->has_child == false)
			return NULL; //MAX_DEPTH이므로 더 나눌 수 없음

		else if (current->depth == MAX_DEPTH && current->has_child == true)
			exit(-1); //에러 (MAX_DEPTH인데 자식이 있을 수는 없음)
	}

	void save() //트리 정보를 파일로 내보내기
	{
		ofstream fout("QUADTREE.txt");
		root->print_pattern(fout);
	}
};

class CTU
{
public:
	int size = CTU_SIZE;

	QUADTREE quadtree;
	//1개의 CTU는 1개의 QUADTREE를 가진다 (싱글턴)

	void RDO()
	{
		QUADTREENODE* candidate;

		while (true)
		{
			candidate = quadtree.FindCandidate(quadtree.root);

			if (candidate == NULL) //모든 Node에 대해 RDO를 할 때까지 반복
				break;

			double result_without_split = candidate->RDOwithoutSplit();
			double result_with_split = candidate->RDOwithSplit(); //시험 삼아 나눠 봄 (자식이 4개 생성됨)

			if (result_without_split <= result_with_split) //안 나누는 게 더 좋음 (RDO 결과물은 작을수록 좋음)
				candidate->Split_undo(); //자식 4개를 전부 지움

			else //나누는 게 더 좋음
			{
				//cout << "SPLITTED" << endl;
				//cout << candidate << endl;
				//cout << candidate->top_left->pattern << endl;
				//cout << candidate->top_right->pattern << endl;
				//cout << candidate->bottom_left->pattern << endl;
				//cout << candidate->bottom_right->pattern << endl;
			}
		}
	}
};

int main()
{
	CTU ctu;

	ctu.RDO();
	ctu.quadtree.save();

	return 0;
}