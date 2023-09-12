#include <iostream>
#include <fstream>
using namespace std;

//======== File I/O ===============
#define InputFile "BasketballDrill.yuv"
#define InputBitDepth 8 //Input bitdepth
#define InputChromaFormat 420 //Ratio of luminance to chrominance samples
#define FrameRate 50 //Frame Rate per second
#define FrameSkip 0 //Number of frames to be skipped in input
#define SourceWidth 832 //Input  frame width
#define SourceHeight 480 //Input  frame height
#define FramesToBeEncoded 500 //Number of frames to be coded
#define Level 3.1

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

    for(unsigned int current_frame = 0; current_frame <= FramesToBeEncoded; current_frame++)
    {

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