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
	//��Ʈ ����� ���, 0�̴�
	//��Ʈ ��尡 �ƴ� ���, 1, 2 ... MAX_DEPTH �̴�

	bool has_child = false;
	QUADTREENODE* top_left = NULL, * top_right = NULL, * bottom_left = NULL, * bottom_right = NULL;
	//�ܸ� ����� ���, NULL�̴� (has_child = true)
	//�ܸ� ��尡 �ƴ� ���, 4���� QUADTREE�� ����Ų�� (has_child = false)

	CU* p_cu = NULL;
	//�ܸ� ���� 1���� CU�� ����Ų��
	//�ܸ� ��尡 �ƴ� ���, NULL�̴�

	bool had_RDO = false;
	//RDO�� �õ��� ���� �ִٸ�, true�� ��

	string pattern = "";
	//��Ʈ ����� ���, ""
	//��Ʈ ��尡 �ƴ� ���, "q", "qw", "qwa" ������ ����

	QUADTREENODE() //��Ʈ ����� ���� ������
	{
		;
	}

	QUADTREENODE(int depth, string pattern) //��Ʈ ��尡 �ƴ� ���� ������
	{
		this->depth = depth;
		this->pattern = pattern;
	}

	//�Ҹ��� �ʿ� ���� (�ڽ��� �ڽ��� ����)

	double RDOwithoutSplit()
	{
		double result = 0;
		return result;
	}

	double RDOwithSplit() //�ܸ� ��尡 �ƴ� ���, �ڽ� 4���� ����. �׸��� RDO ������ ��
	{
		if (has_child == false)
			has_child = true;

		else if (has_child == true)
			exit(-2); //�̹� �����µ� �� ���� ���� ����

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

	QUADTREE() //������
	{
		root = new QUADTREENODE();
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
		ofstream fout("QUADTREE.txt");
		root->print_pattern(fout);
	}
};

class CTU
{
public:
	int size = CTU_SIZE;

	QUADTREE quadtree;
	//1���� CTU�� 1���� QUADTREE�� ������ (�̱���)

	void RDO()
	{
		QUADTREENODE* candidate;

		while (true)
		{
			candidate = quadtree.FindCandidate(quadtree.root);

			if (candidate == NULL) //��� Node�� ���� RDO�� �� ������ �ݺ�
				break;

			double result_without_split = candidate->RDOwithoutSplit();
			double result_with_split = candidate->RDOwithSplit(); //���� ��� ���� �� (�ڽ��� 4�� ������)

			if (result_without_split <= result_with_split) //�� ������ �� �� ���� (RDO ������� �������� ����)
				candidate->Split_undo(); //�ڽ� 4���� ���� ����

			else //������ �� �� ����
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