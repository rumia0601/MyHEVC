#include <iostream>
#include <fstream>

#include "Macro.h"
#include "Block.h"
#include "DPB.h"
using namespace std;

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

    // 파일을 쓰기 모드로 열기
    ofstream test_file("Test_BasketballDrill.yuv", ios::binary);

    char byte;
    unsigned int count_of_frame = 0;
    unsigned int count_of_video = 0;
    unsigned int max_count_of_frame = SourceWidth * SourceHeight * 1.5; //한 프레임은 832 x 480 x 1.5 픽셀로 구성
    unsigned int max_count_of_video = FramesToBeEncoded; //동영상은 500 프레임으로 구성

    unsigned int current_CTU_col = 0;

    Picture_Block picture_block;
    //cout << "DONE" << endl;

    for (unsigned int current_frame = 1; current_frame <= FramesToBeEncoded; current_frame++) //1번째 프레임부터 500번째 프레임까지
    {
        //새로운 프레임

        for (unsigned int current_CTU_row = 0; current_CTU_row < CTU_Y_COUNT_ROW ; current_CTU_row++)
            for (unsigned int current_CTU_col = 0; current_CTU_col < CTU_Y_COUNT_COL; current_CTU_col++)
            {
                double RDO_CTU = 0;
            }
        //Y
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