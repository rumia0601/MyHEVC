#include <iostream>
#include <fstream>

#include "Macro.h"
#include "Block.h"
#include "DPB.h"
using namespace std;

int main()
{
    // ���� �̸� ����
    const char* filename = InputFile;

    // ������ �б� ���� ����
    ifstream file(filename, ios::binary);

    // ���� ���� ���� Ȯ��
    if (!file)
    {
        cout << "Input yuv file don't exist." << endl;
        return -1; // ���� �ڵ� ��ȯ
    }

    // ������ ���� ���� ����
    ofstream test_file("Test_BasketballDrill.yuv", ios::binary);

    char byte;
    unsigned int count_of_frame = 0;
    unsigned int count_of_video = 0;
    unsigned int max_count_of_frame = SourceWidth * SourceHeight * 1.5; //�� �������� 832 x 480 x 1.5 �ȼ��� ����
    unsigned int max_count_of_video = FramesToBeEncoded; //�������� 500 ���������� ����

    unsigned int current_CTU_col = 0;

    Picture_Block picture_block;
    //cout << "DONE" << endl;

    for (unsigned int current_frame = 1; current_frame <= FramesToBeEncoded; current_frame++) //1��° �����Ӻ��� 500��° �����ӱ���
    {
        //���ο� ������

        for (unsigned int current_CTU_row = 0; current_CTU_row < CTU_Y_COUNT_ROW ; current_CTU_row++)
            for (unsigned int current_CTU_col = 0; current_CTU_col < CTU_Y_COUNT_COL; current_CTU_col++)
            {
                double RDO_CTU = 0;
            }
        //Y
    }

    /*
    // 16���� ��� ����
    cout << hex;

    // ���Ͽ��� 1����Ʈ�� �о ���
    char byte;
    while (file.get(byte))
    {
        cout << static_cast<int>(byte);
    }

    // 16���� ��� ���� ���� (�⺻���� ���ư�)
    cout << dec;
    */

    // ���� ��Ʈ�� �ݱ�
    file.close();

    return 0;
}