#pragma once
#include "Struct.h"
#include "Calculate.h"
#include "DataProcess.h"

using namespace std;

bool Mapping();
void ErrorMsg(bool type, string code, int index, string message);
// SYNTAX
// [group1]
// n1[12.4,12.0],mode,n2/n3/n4/n5/n6/1321;12312/group2[partial]
// n2[],0,n1
// [group2]
// n1[1.0,2.0],mode,n1/group1;n2

// 첫번째 항 : neuron의 id, 임의 명칭 지정 가능 괄호안 [threshold, weight], 괄호는 생략 가능, 그룹별로 id는 별개로 취급
// 두번째 항 : 주소 등록 방식(공백은 뒤에 있는 주소를 이용, 'x'는 주소 없이 다음 neuron으로 전달, 'i'는 input neuron으로 사용, 'o'은 output peuron으로 사용)
// 세번째 항 : 주소 목록(형식 : neuron id 또는 page;sector 또는 group)


// TODO
// group 등 필요한 정보 파일로 저장
// 상호 참조 없도록 설계 n1->n2 n2->n1 불가능
// 문법적으로 옳지 않으면 오류 출력 => 오류는 log파일로 저장