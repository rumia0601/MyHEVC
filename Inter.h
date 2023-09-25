#pragma once

//인터 예측

class MV //Inter인 블록의 움직임 정보
{
public:

};

class Inter
{
public:
	unsigned char mode = 0; //1 ~ 3 (1 = AMVP, 2 = MERGE, 3 = SKIP)
	//0은 안 쓰는 값 (초기화 안한 상태)
	MV mv;
	//         AMVP MERGE SKIP
	//MV       X    X     X (MP = *pMVP + MVD. 전송 대상은 아니지만 이 값은 디코더에서도 동일히 복원됨)
	//pMVP     O    O     O
	//MVD      O    X     X
	//residual O    O     X
};