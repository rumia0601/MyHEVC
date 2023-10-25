#include <iostream>
#include <string>
#include <fstream>
#include <stack>
#include <random>
#include <vector>
using namespace std;

#define CTU_SIZE 64
#define MAX_CU_SIZE 64
#define MIN_CU_SIZE 8
#define MIN_DEPTH 0
#define MAX_DEPTH 3

#define ROW 480
#define COL 832
#define ROW_PADDED 512 
#define COL_PADDED 832

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dis(0.0, 1.0);
//�յ� ���� ����

ifstream fin("BasketballDrill.yuv", ios::binary);
ofstream fout_txt("QUADTREE.txt");
ofstream fout("QUADTREE.yuv", ios::binary);
ofstream fout_not_flatten("QUADTREE_NOT_FLATTEN.yuv", ios::binary);
unsigned char current_frame[ROW_PADDED][COL_PADDED];
unsigned char current_frame_reconstructed[ROW_PADDED][COL_PADDED];

class CU
{
public:
	int row = 0;
	int col = 0;
	int size = 0;
	//�� CU�� [row][col]        ... [row][col + size]
	//        ...                   ...
	//        [row + size][col] ... [row + size][col + size] �� �����ȴ�

	vector<vector<unsigned char>> current_block;

	CU(int row, int col, int size)
	{
		this->row = row;
		this->col = col;
		this->size = size;
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
				//cout << i << " " << j << " " << current_block[i][j];
			}
		}
		//���� �������� �ȼ��� �� ���� ��ϰ� �����Ǵ� �ȼ�����, ���� ��Ͽ� ����
	}

	void print_block()
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				fout << current_block[i][j];
				//cout << current_block[i][j];

				current_frame_reconstructed[row + i][col + j] = current_block[i][j];
			}
		}
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

		result = cu->predict();

		return result;
	}

	double RDOwithSplit() //�ܸ� ��尡 �ƴ� ���, �ڽ� 4���� ����. �׸��� RDO ������ ��
	{
		if (has_child == false)
			has_child = true;

		else if (has_child == true)
			exit(-2); //�̹� �����µ� �� ���� ���� ����

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
			exit(-3); //RDO�� �غôµ� �ڽ��� ���� ���� ����

		if (had_RDO == false)
			had_RDO = true;

		else if (had_RDO == true)
			exit(-4); //RDO�� �� �غôµ� undo�� �� ���� ����

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
				//cout << "x" << " " << row << " " << col << " " << size << endl;

				fout_txt << "x" << " " << row << " " << col << " " << size << endl;
			}

			else
			{
				//cout << pattern << " " << row << " " << col << " " << size << endl;

				fout_txt << pattern << " " << row << " " << col << " " << size << endl;
			}
		}
	}

	void print_block()
	{
		if (top_left != NULL)
			top_left->print_block();
		if (top_right != NULL)
			top_right->print_block();
		if (bottom_left != NULL)
			bottom_left->print_block();
		if (bottom_right != NULL)
			bottom_right->print_block();

		if (has_child == false)
		{
			cu->print_block();
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

		root->print_block();
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

			result_without_split *= dis(gen); //1 * [0, 1)
			result_with_split *= (0.25 * dis(gen)); //4 * 0.25 * [0, 1)

			//cout << result_without_split << " " << result_with_split << endl;
			//������ ����ȭ

			if (result_without_split <= result_with_split) //�� ������ �� �� ���� (RDO ������� �������� ����)
			{
				//cout << "not split" << endl;

				candidate->Split_undo(); //�ڽ� 4���� ���� ����
			}

			else //������ �� �� ����
			{
				//cout << "split" << endl;

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
	//fin.read(reinterpret_cast<char*>(&current_frame[0][0]), ROW_PADDED * COL_PADDED); //0th frame of file -> [480][832]

	for (int i = 0; i < ROW; i++)
		for (int j = 0; j < COL; j++)
			fin.get(reinterpret_cast<char&>(current_frame[i][j]));
	//current frame ����

	for (int i = 0; i < ROW; i+= CTU_SIZE)
		for (int j = 0; j < COL; j += CTU_SIZE)
		{
			CTU ctu(i, j, CTU_SIZE);

			ctu.RDO();
			ctu.quadtree->save();
		}
	//��� CTU�� ���ؼ�, RDO�� ����

	for (int i = 0; i < ROW_PADDED; i++)
		for (int j = 0; j < COL_PADDED; j++)
		{
			fout_not_flatten << current_frame_reconstructed[i][j];
		}
	//������ ������ ����

	return 0;
}