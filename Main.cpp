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
    // 파일 이름 설정
    const char* filename = InputFile;

    // 파일을 읽기 모드로 열기
    ifstream file(filename, ios::binary);

    // 파일 열기 실패 확인
    if (!file)
    {
        cout << "Input yuv file don't exist." << endl;
        return -1; // 에러 코드 반환
    }

    for(unsigned int current_frame = 0; current_frame <= FramesToBeEncoded; current_frame++)
    {

    }

    /*
    // 16진수 출력 설정
    cout << hex;

    // 파일에서 1바이트씩 읽어서 출력
    char byte;
    while (file.get(byte))
    {
        cout << static_cast<int>(byte);
    }

    // 16진수 출력 설정 해제 (기본으로 돌아감)
    cout << dec;
    */

    // 파일 스트림 닫기
    file.close();

    return 0;
}