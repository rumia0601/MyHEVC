#pragma once

//���� ����

class MV //Inter�� ����� ������ ����
{
public:

};

class Inter
{
public:
	unsigned char mode = 0; //1 ~ 3 (1 = AMVP, 2 = MERGE, 3 = SKIP)
	//0�� �� ���� �� (�ʱ�ȭ ���� ����)
	MV mv;
	//         AMVP MERGE SKIP
	//MV       X    X     X (MP = *pMVP + MVD. ���� ����� �ƴ����� �� ���� ���ڴ������� ������ ������)
	//pMVP     O    O     O
	//MVD      O    X     X
	//residual O    O     X
};