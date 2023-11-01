#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <random>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

#define CTU_SIZE 64
#define MAX_CU_SIZE 64
#define MIN_CU_SIZE 8
#define MIN_DEPTH 0
#define MAX_DEPTH 3

#define ROW 480
#define COL 832
#define ROW_CB 240
#define COL_CB 416
#define ROW_CR 240
#define COL_CR 416
#define FRAME 500
#define ROW_PADDED 512 
#define COL_PADDED 832

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0.0, 1.0);
//균등 분포 생성

ifstream fin("BasketballDrill.yuv", ios::binary);
ofstream fout_txt("PREDICT.txt");

ofstream fout_predict("PREDICT.yuv", ios::binary);
ofstream fout_residual("RESIDUAL.yuv", ios::binary);
ofstream fout_reconstruct("RECONSTRUCT.yuv", ios::binary);

unsigned char current_frame[ROW_PADDED][COL_PADDED];
unsigned char current_frame_reconstruct[ROW_PADDED][COL_PADDED];

unsigned char current_frame_predict[ROW_PADDED][COL_PADDED];
signed short current_frame_residual[ROW_PADDED][COL_PADDED];

int without_split = 0;
int with_split = 0;

class CU
{
public:
	int row = 0;
	int col = 0;
	int size = 0;
	//이 CU는 [row][col]        ... [row][col + size]
	//        ...                   ...
	//        [row + size][col] ... [row + size][col + size] 에 대응된다

	unsigned int DC = 0;
	//현재 CU의 DC 성분 (CU내 모든 픽셀들의 평균)

	vector<vector<unsigned char>> current_block;

	CU(int row, int col, int size)
	{
		this->row = row;
		this->col = col;
		this->size = size;
		this->DC = 0;
		set_block();
	}

	void set_block()
	{
		current_block.resize(size, vector<unsigned char>(size));

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				current_block[i][j] = current_frame[row + i][col + j];
				DC += current_block[i][j];
			}
		}
		DC /= (size * size);

		//현재 프레임의 픽셀들 중 현재 블록과 대응되는 픽셀들을, 현재 블록에 복사
	}

	void reconstruct()
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				signed short current = 0;
				
				unsigned char predict = current_frame_predict[row + i][col + j]; //[0, 255]
				signed short residual = current_frame_residual[row + i][col + j]; //[-255, 255]

				current = residual;
				current += predict;
				//이론상 [-255, 510]지만, 실제로는 [0, 255]만이 나옴
				
				if (current <= -1 || current >= 256)
				{
					exit(-1);
				}
				//무조건 [0, 255]여야 함

				current_frame_reconstruct[row + i][col + j] = current;
			}
		}
	}

	void set_predict()
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				current_frame_predict[row + i][col + j] = DC;
				//일단, DC로만 예측
			}
		}
	}

	void set_residual()
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				unsigned char original = current_block[i][j];
				unsigned char predict = current_frame_predict[row + i][col + j];
				signed short delta = original - predict;

				current_frame_residual[row + i][col + j] = delta;
				//current_block = [0, 255]
				//predict = [0, 255]
				//residual = [-255, 255]
			}
		}
	}

	long long set_predict_residual()
	{
		set_predict();
		set_residual();

		long long rate = 0;

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				rate += abs(current_frame_residual[row + i][col + j]); //5697 5715
				//rate += (current_frame_residual[row + i][col + j]) * (current_frame_residual[row + i][col + j]);
			}
		}
		//무손실이므로, distortion = 0 (실제로는 transform을 해서 얻어 봐야 함)
		//rate는 residual의 절댓값의 합의 평균 (실제로는 CABAC을 해서 얻어 봐야 함)
		//abs : 3260 22756
		//se : 2296 28784

		return rate; 
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

		result = cu->set_predict_residual();

		return result;
	}

	double RDOwithSplit() //단말 노드가 아닌 경우, 자식 4개를 만듦. 그리고 RDO 점수를 냄
	{
		if (has_child == false)
			has_child = true;

		else if (has_child == true)
			exit(-1); //이미 나눴는데 또 나눌 수는 없음

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
			exit(-1); //RDO를 해봤는데 자식이 없을 수는 없음

		if (had_RDO == false)
			had_RDO = true;

		else if (had_RDO == true)
			exit(-1); //RDO를 안 해봤는데 undo를 할 수는 없음

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

		cu->set_predict();
		cu->set_residual();
		//전역 자료구조인 predict, residual이 자식 노드들이 RDO를 하면서 수정되었으므로 되돌려야 함
	}

	void print_pattern()
	{
		if (top_left != NULL)
			top_left->print_pattern();
		if (top_right != NULL)
			top_right->print_pattern();
		if (bottom_left != NULL)
			bottom_left->print_pattern();
		if (bottom_right != NULL)
			bottom_right->print_pattern();

		if (has_child == false)
		{
			if (depth == 0) //CTU = CU인 경우
			{
				fout_txt << "x" << " " << row << " " << col << " " << size << endl;
			}

			else
			{
				fout_txt << pattern << " " << row << " " << col << " " << size << endl;
			}
		}
	}

	void reconstruct()
	{
		if (top_left != NULL)
			top_left->reconstruct();
		if (top_right != NULL)
			top_right->reconstruct();
		if (bottom_left != NULL)
			bottom_left->reconstruct();
		if (bottom_right != NULL)
			bottom_right->reconstruct();

		if (has_child == false)
		{
			cu->reconstruct();
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
		root->print_pattern();
		fout_txt << endl;

		root->reconstruct();
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

			//result_without_split = dis(gen);
			//result_with_split = dis(gen);
			//점수를 랜덤화

			if (result_without_split <= result_with_split)
				without_split++;
			else
				with_split++;

			if (result_without_split <= result_with_split) //안 나누는 게 더 좋음 (RDO 결과물은 작을수록 좋음)
			{
				candidate->Split_undo(); //자식 4개를 전부 지움
			}

			else //나누는 게 더 좋음
			{
				;
			}
		}
	}
};

int main()
{
	for (int k = 0; k < FRAME; k++)
	{
		cout << k << endl;

		//fin.seekg(ROW * COL, std::ios::cur); //Y를 스킵할 때
		fill(&current_frame[0][0], &current_frame[0][0] + ROW * COL, 0); //Y를 할 때

		for (int i = 0; i < ROW; i++)
			for (int j = 0; j < COL; j++)
				fin.get(reinterpret_cast<char&>(current_frame[i][j]));
		//현재 프레임의 Y 불러오기

		for (int i = 0; i < ROW; i += CTU_SIZE)
			for (int j = 0; j < COL; j += CTU_SIZE)
			{
				CTU ctu(i, j, CTU_SIZE);

				ctu.RDO();
				ctu.quadtree->save();
			}
		//현재 프레임의 Y CTU에 대해서 RDO를 진행

		for (int i = 0; i < ROW; i++)
			for (int j = 0; j < COL; j++)
			{
				fout_predict << current_frame_predict[i][j];

				signed short current_short = current_frame_residual[i][j]; //[-255, 255]
				current_short += (signed short) 255; //[0, 510]
				current_short = (current_short + 1) >> 1; //[0, 255]
				unsigned char current_char = current_short; //[0, 255]
				fout_residual << current_char;

				fout_reconstruct << current_frame_reconstruct[i][j];
			}
		//현재 프레임의 Y에 대해서 prediction, residual, reconstruct을 파일로 save

		//fin.seekg(ROW_CB * COL_CB, std::ios::cur); //Cb를 스킵할 때
		fill(&current_frame[0][0], &current_frame[0][0] + ROW * COL, 0); //Cb를 할 때

		for (int i = 0; i < ROW_CB; i++)
			for (int j = 0; j < COL_CB; j++)
				fin.get(reinterpret_cast<char&>(current_frame[i][j]));
		//현재 프레임의 Cb 불러오기

		for (int i = 0; i < ROW_CB; i += CTU_SIZE)
			for (int j = 0; j < COL_CB; j += CTU_SIZE)
			{
				CTU ctu(i, j, CTU_SIZE);

				ctu.RDO();
				ctu.quadtree->save();
			}
		//현재 프레임의 Cb CTU에 대해서 RDO를 진행

		for (int i = 0; i < ROW_CB; i++)
			for (int j = 0; j < COL_CB; j++)
			{
				fout_predict << current_frame_predict[i][j];

				signed short current_short = current_frame_residual[i][j]; //[-255, 255]
				current_short += (signed short)255; //[0, 510]
				current_short = (current_short + 1) >> 1; //[0, 255]
				unsigned char current_char = current_short; //[0, 255]
				fout_residual << current_char;

				fout_reconstruct << current_frame_reconstruct[i][j];
			}
		//현재 프레임의 Cb에 대해서 prediction, residual, reconstruct을 파일로 save

		//fin.seekg(ROW_CR * COL_CR, std::ios::cur); //Cr을 스킵할 때
		fill(&current_frame[0][0], &current_frame[0][0] + ROW * COL, 0); //Cr을 할 때

		for (int i = 0; i < ROW_CR; i++)
			for (int j = 0; j < COL_CR; j++)
				fin.get(reinterpret_cast<char&>(current_frame[i][j]));
		//현재 프레임의 Cr 불러오기

		for (int i = 0; i < ROW_CR; i += CTU_SIZE)
			for (int j = 0; j < COL_CR; j += CTU_SIZE)
			{
				CTU ctu(i, j, CTU_SIZE);

				ctu.RDO();
				ctu.quadtree->save();
			}
		//현재 프레임의 Cr CTU에 대해서 RDO를 진행

		for (int i = 0; i < ROW_CR; i++)
			for (int j = 0; j < COL_CR; j++)
			{
				fout_predict << current_frame_predict[i][j];

				signed short current_short = current_frame_residual[i][j]; //[-255, 255]
				current_short += (signed short)255; //[0, 510]
				current_short = (current_short + 1) >> 1; //[0, 255]
				unsigned char current_char = current_short; //[0, 255]
				fout_residual << current_char;

				fout_reconstruct << current_frame_reconstruct[i][j];
			}
		//현재 프레임의 Cr에 대해서 prediction, residual, reconstruct을 파일로 save
	}
	fin.close();
	fout_txt.close();
	fout_predict.close();
	fout_residual.close();
	fout_reconstruct.close();
	//모든 프레임의 CTU에 대해서 RDO를 진행, 복원된 프레임 생성

	cout << without_split << " " << with_split << endl;

	return 0;
}