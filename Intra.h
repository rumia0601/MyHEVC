#pragma once

//인트라 예측

class Intra
{
public:
	unsigned char mode = 0; //1 ~ 35 (intra의 1번 모드부터 35번 모드까지)
	//0은 안 쓰는 값 (초기화 안한 상태)
};
