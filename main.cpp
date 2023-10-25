#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <random>
using namespace std;

#define CTU_SIZE 64
#define MAX_CU_SIZE 64
#define MIN_CU_SIZE 8
#define MIN_DEPTH 0
#define MAX_DEPTH 3

#define ROW 480
#define COL 832

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0.0, 1.0);
//균등 분포 생성

ifstream fin("BasketballDrill.yuv");
unsigned char current_frame[480][832];

class CU
{
public:
	int row = 0;
	int col = 0;
	int size = 0;
	//이 CU는 [row][col]        ... [row][col + size]
	//        ...                   ...
	//        [row + size][col] ... [row + size][col + size] 에 대응된다

	CU(int row, int col, int size)
	{
		this->row = row;
		this->col = col;
		this->size = size;
	}

	double predict()
	{
		return 1.0;
	}
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

	CU* cu = NULL;
	//단말 노드는 1개의 CU를 가르킨다
	//단말 노드가 아닌 경우, NULL이다

	bool had_RDO = false;
	//RDO를 시도한 적이 있다면, true가 됨

	string pattern = "";
	//루트 노드인 경우, ""
	//루트 노드가 아닌 경우, "q", "qw", "qwa" 등으로 존재

	int row = 0;
	int col = 0;
	int size = 0;

	QUADTREENODE(int row, int col, int size, int depth, string pattern)
	{
		this->row = row;
		this->col = col;
		this->size = size;

		this->depth = depth;
		this->pattern = pattern;

		this->cu = new CU(row, col, size); //CU 생성
	}

	~QUADTREENODE()
	{
		if (cu != NULL)
			delete cu;

		if (top_left != NULL)
			delete top_left;
		if (top_right != NULL)
			delete top_right;
		if (bottom_left != NULL)
			delete bottom_left;
		if (bottom_right != NULL)
			delete bottom_right;
	}

	double RDOwithoutSplit()
	{
		double result = 0;

		result = cu->predict();

		return result;
	}

	double RDOwithSplit() //단말 노드가 아닌 경우, 자식 4개를 만듦. 그리고 RDO 점수를 냄
	{
		if (has_child == false)
			has_child = true;

		else if (has_child == true)
			exit(-2); //이미 나눴는데 또 나눌 수는 없음

		int half_size = size / 2;

		delete cu;
		cu = NULL;
		//말단 노드가 아니게 되었으므로 CU를 삭제

		top_left = new QUADTREENODE(row, col, half_size, depth + 1, pattern + 'q');
		top_right = new QUADTREENODE(row, col + half_size, half_size, depth + 1, pattern + 'w');
		bottom_left = new QUADTREENODE(row + half_size, col, half_size, depth + 1, pattern + 'a');
		bottom_right = new QUADTREENODE(row + half_size, col + half_size, half_size, depth + 1, pattern + 's');
		//말단 노드가 아니게 되었으므로 자식 노드들을 생성

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

		this->cu = new CU(row, col, size);
		//말단 노드가 되었으므로 CU 생성

		delete top_left;
		top_left = NULL;
		delete top_right;
		top_right = NULL;
		delete bottom_left;
		bottom_left = NULL;
		delete bottom_right;
		bottom_right = NULL;
		//말단 노드가 되었으므로 자식들을 삭제

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
		{
			if (depth == 0) //CTU = CU인 경우
				fout << "x" << " " << row << " " << col << " " << size << endl;

			else
				fout << pattern << " " << row << " " << col << " " << size << endl;
		}
	}
};

class QUADTREE
{
public:
	QUADTREENODE *root = NULL;

	int row = 0;
	int col = 0;
	int size = 0;

	QUADTREE(int row, int col, int size) //생성자
	{
		this->row = row;
		this->col = col;
		this->size = size;

		root = new QUADTREENODE(row, col, size, 0, "");
	}

	~QUADTREE()
	{
		if (root != NULL)
			delete root;
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
	int row = 0;
	int col = 0;
	int size = 0;
	//이 CTU는 [row][col]        ... [row][col + size]
	//         ...                   ...
	//         [row + size][col] ... [row + size][col + size] 에 대응된다

	QUADTREE* quadtree;
	//1개의 CTU는 1개의 QUADTREE를 가진다 (싱글턴)

	CTU(int row, int col, int size)
	{
		this->row = row;
		this->col = col;
		this->size = size;
		quadtree = new QUADTREE(row, col, size);
	}

	~CTU()
	{
		if (quadtree != NULL)
			delete quadtree;
	}

	void RDO()
	{
		QUADTREENODE* candidate;

		while (true)
		{
			candidate = quadtree->FindCandidate(quadtree->root);

			if (candidate == NULL) //모든 Node에 대해 RDO를 할 때까지 반복
				break;

			double result_without_split = candidate->RDOwithoutSplit();
			double result_with_split = candidate->RDOwithSplit(); //시험 삼아 나눠 봄 (자식이 4개 생성됨)

			result_without_split *= dis(gen); //1 * [0, 1)
			result_with_split *= (0.25 * dis(gen)); //4 * 0.25 * [0, 1)

			cout << result_without_split << " " << result_with_split << endl;
			//점수를 난수화

			if (result_without_split <= result_with_split) //안 나누는 게 더 좋음 (RDO 결과물은 작을수록 좋음)
			{
				cout << "not split" << endl;

				candidate->Split_undo(); //자식 4개를 전부 지움
			}

			else //나누는 게 더 좋음
			{
				cout << "split" << endl;

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
	fin.read(reinterpret_cast<char*>(&current_frame[0][0]), ROW * COL); //0th frame of file -> [480][832]

	CTU ctu(0, 0, CTU_SIZE);

	ctu.RDO();
	ctu.quadtree->save();

	return 0;
}