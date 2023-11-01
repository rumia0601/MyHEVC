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
//�յ� ���� ����

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
	//�� CU�� [row][col]        ... [row][col + size]
	//        ...                   ...
	//        [row + size][col] ... [row + size][col + size] �� �����ȴ�

	unsigned int DC = 0;
	//���� CU�� DC ���� (CU�� ��� �ȼ����� ���)

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

		//���� �������� �ȼ��� �� ���� ��ϰ� �����Ǵ� �ȼ�����, ���� ��Ͽ� ����
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
				//�̷л� [-255, 510]����, �����δ� [0, 255]���� ����
				
				if (current <= -1 || current >= 256)
				{
					exit(-1);
				}
				//������ [0, 255]���� ��

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
				//�ϴ�, DC�θ� ����
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
		//���ս��̹Ƿ�, distortion = 0 (�����δ� transform�� �ؼ� ��� ���� ��)
		//rate�� residual�� ������ ���� ��� (�����δ� CABAC�� �ؼ� ��� ���� ��)
		//abs : 3260 22756
		//se : 2296 28784

		return rate; 
	}
};

class QUADTREENODE
{
public:
	int depth = 0;
	//��Ʈ ����� ���, 0�̴�
	//��Ʈ ��尡 �ƴ� ���, 1, 2 ... MAX_DEPTH �̴�

	bool has_child = false;
	QUADTREENODE* top_left = NULL, * top_right = NULL, * bottom_left = NULL, * bottom_right = NULL;
	//�ܸ� ����� ���, NULL�̴� (has_child = true)
	//�ܸ� ��尡 �ƴ� ���, 4���� QUADTREE�� ����Ų�� (has_child = false)

	CU* cu = NULL;
	//�ܸ� ���� 1���� CU�� ����Ų��
	//�ܸ� ��尡 �ƴ� ���, NULL�̴�

	bool had_RDO = false;
	//RDO�� �õ��� ���� �ִٸ�, true�� ��

	string pattern = "";
	//��Ʈ ����� ���, ""
	//��Ʈ ��尡 �ƴ� ���, "q", "qw", "qwa" ������ ����

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

		this->cu = new CU(row, col, size); //CU ����
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

	double RDOwithSplit() //�ܸ� ��尡 �ƴ� ���, �ڽ� 4���� ����. �׸��� RDO ������ ��
	{
		if (has_child == false)
			has_child = true;

		else if (has_child == true)
			exit(-1); //�̹� �����µ� �� ���� ���� ����

		int half_size = size / 2;

		delete cu;
		cu = NULL;
		//���� ��尡 �ƴϰ� �Ǿ����Ƿ� CU�� ����

		top_left = new QUADTREENODE(row, col, half_size, depth + 1, pattern + 'q');
		top_right = new QUADTREENODE(row, col + half_size, half_size, depth + 1, pattern + 'w');
		bottom_left = new QUADTREENODE(row + half_size, col, half_size, depth + 1, pattern + 'a');
		bottom_right = new QUADTREENODE(row + half_size, col + half_size, half_size, depth + 1, pattern + 's');
		//���� ��尡 �ƴϰ� �Ǿ����Ƿ� �ڽ� ������ ����

		double result1 = top_left->RDOwithoutSplit();
		double result2 = top_right->RDOwithoutSplit();
		double result3 = bottom_left->RDOwithoutSplit();
		double result4 = bottom_right->RDOwithoutSplit();
		double result = result1 + result2 + result3 + result4;

		return result;
	}

	void Split_undo() //���� �ڽ� 4���� �ٽ� ���� (�ѹ�)
	{
		if (has_child == true)
			has_child = false;

		else if (has_child == false)
			exit(-1); //RDO�� �غôµ� �ڽ��� ���� ���� ����

		if (had_RDO == false)
			had_RDO = true;

		else if (had_RDO == true)
			exit(-1); //RDO�� �� �غôµ� undo�� �� ���� ����

		this->cu = new CU(row, col, size);
		//���� ��尡 �Ǿ����Ƿ� CU ����

		delete top_left;
		top_left = NULL;
		delete top_right;
		top_right = NULL;
		delete bottom_left;
		bottom_left = NULL;
		delete bottom_right;
		bottom_right = NULL;
		//���� ��尡 �Ǿ����Ƿ� �ڽĵ��� ����

		top_left = NULL;
		top_right = NULL;
		bottom_left = NULL;
		bottom_right = NULL;

		cu->set_predict();
		cu->set_residual();
		//���� �ڷᱸ���� predict, residual�� �ڽ� ������ RDO�� �ϸ鼭 �����Ǿ����Ƿ� �ǵ����� ��
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
			if (depth == 0) //CTU = CU�� ���
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

	QUADTREE(int row, int col, int size) //������
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

	QUADTREENODE* FindCandidate(QUADTREENODE* current) //depth�� MAX_DEPTH�� �ƴϰ� had_RDO�� false�� QUADTREENODE�� ��, DFS ���� �� ���� ���� ������ QUADTRENODE
	{
		if (current->depth != MAX_DEPTH && current->has_child == false && current->had_RDO == false)
			return current;

		else if (current->depth != MAX_DEPTH && current->has_child == false && current->had_RDO == true)
			return NULL;

		else if (current->depth != MAX_DEPTH && current->has_child == true)
		{
			QUADTREENODE* result1 = FindCandidate(current->top_left); //���� ���� �� ���� �� �ִ� �� Ȯ��
			if (result1 != NULL) //�� ���� �� ����
				return result1; //���� ���� ��ȯ

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
			return NULL; //MAX_DEPTH�̹Ƿ� �� ���� �� ����

		else if (current->depth == MAX_DEPTH && current->has_child == true)
			exit(-1); //���� (MAX_DEPTH�ε� �ڽ��� ���� ���� ����)
	}

	void save() //Ʈ�� ������ ���Ϸ� ��������
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
	//�� CTU�� [row][col]        ... [row][col + size]
	//         ...                   ...
	//         [row + size][col] ... [row + size][col + size] �� �����ȴ�

	QUADTREE* quadtree;
	//1���� CTU�� 1���� QUADTREE�� ������ (�̱���)

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

			if (candidate == NULL) //��� Node�� ���� RDO�� �� ������ �ݺ�
				break;

			double result_without_split = candidate->RDOwithoutSplit();
			double result_with_split = candidate->RDOwithSplit(); //���� ��� ���� �� (�ڽ��� 4�� ������)

			//result_without_split = dis(gen);
			//result_with_split = dis(gen);
			//������ ����ȭ

			if (result_without_split <= result_with_split)
				without_split++;
			else
				with_split++;

			if (result_without_split <= result_with_split) //�� ������ �� �� ���� (RDO ������� �������� ����)
			{
				candidate->Split_undo(); //�ڽ� 4���� ���� ����
			}

			else //������ �� �� ����
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

		//fin.seekg(ROW * COL, std::ios::cur); //Y�� ��ŵ�� ��
		fill(&current_frame[0][0], &current_frame[0][0] + ROW * COL, 0); //Y�� �� ��

		for (int i = 0; i < ROW; i++)
			for (int j = 0; j < COL; j++)
				fin.get(reinterpret_cast<char&>(current_frame[i][j]));
		//���� �������� Y �ҷ�����

		for (int i = 0; i < ROW; i += CTU_SIZE)
			for (int j = 0; j < COL; j += CTU_SIZE)
			{
				CTU ctu(i, j, CTU_SIZE);

				ctu.RDO();
				ctu.quadtree->save();
			}
		//���� �������� Y CTU�� ���ؼ� RDO�� ����

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
		//���� �������� Y�� ���ؼ� prediction, residual, reconstruct�� ���Ϸ� save

		//fin.seekg(ROW_CB * COL_CB, std::ios::cur); //Cb�� ��ŵ�� ��
		fill(&current_frame[0][0], &current_frame[0][0] + ROW * COL, 0); //Cb�� �� ��

		for (int i = 0; i < ROW_CB; i++)
			for (int j = 0; j < COL_CB; j++)
				fin.get(reinterpret_cast<char&>(current_frame[i][j]));
		//���� �������� Cb �ҷ�����

		for (int i = 0; i < ROW_CB; i += CTU_SIZE)
			for (int j = 0; j < COL_CB; j += CTU_SIZE)
			{
				CTU ctu(i, j, CTU_SIZE);

				ctu.RDO();
				ctu.quadtree->save();
			}
		//���� �������� Cb CTU�� ���ؼ� RDO�� ����

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
		//���� �������� Cb�� ���ؼ� prediction, residual, reconstruct�� ���Ϸ� save

		//fin.seekg(ROW_CR * COL_CR, std::ios::cur); //Cr�� ��ŵ�� ��
		fill(&current_frame[0][0], &current_frame[0][0] + ROW * COL, 0); //Cr�� �� ��

		for (int i = 0; i < ROW_CR; i++)
			for (int j = 0; j < COL_CR; j++)
				fin.get(reinterpret_cast<char&>(current_frame[i][j]));
		//���� �������� Cr �ҷ�����

		for (int i = 0; i < ROW_CR; i += CTU_SIZE)
			for (int j = 0; j < COL_CR; j += CTU_SIZE)
			{
				CTU ctu(i, j, CTU_SIZE);

				ctu.RDO();
				ctu.quadtree->save();
			}
		//���� �������� Cr CTU�� ���ؼ� RDO�� ����

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
		//���� �������� Cr�� ���ؼ� prediction, residual, reconstruct�� ���Ϸ� save
	}
	fin.close();
	fout_txt.close();
	fout_predict.close();
	fout_residual.close();
	fout_reconstruct.close();
	//��� �������� CTU�� ���ؼ� RDO�� ����, ������ ������ ����

	cout << without_split << " " << with_split << endl;

	return 0;
}