#include "Mapping.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>

// SYNTAX
// [group1]
// n1[12.4,12.0],mode,n2/n3/n4/n5/n6/1321;12312/group2[partial]
// n2[],0,n1
// [group2]
// n1[1.0,2.0],mode,n1/group1;n2

// 첫번째 항 : neuron의 id, 임의 명칭 지정 가능 괄호안 [threshold, weight], 괄호는 생략 가능, 그룹별로 id는 별개로 취급
// 두번째 항 : group 나누기 (다중 그룹 지정 가능)
// 세번째 항 : 주소 등록 방식(공백은 뒤에 있는 주소를 이용, 'z'는 주소 없이 다음 neuron으로 전달, 'i'는 input neuron으로 사용, 'o'은 output peuron으로 사용)
// 네번째 항 : 주소 목록(형식 : neuron id 또는 page;sector 또는 group)


// TODO
// group 등 필요한 정보 파일로 저장
// 상호 참조 없도록 설계 n1->n2 n2->n1 불가능
// 문법적으로 옳지 않으면 오류 출력 => 오류는 log파일로 저장

struct Group
{
	string name;
	streampos pos;
	int count;
	vector<string> *strings;
};

vector<Group> vector_group;
vector<string> vector_string;

bool Mapping(){
    string address = (string)Path + "mapping";
	ifstream file(address);
	if (file.is_open())
    {
		string str;
		string error;
		streampos pos; // 파일에서의 위치
		int line = 1;
		size_t index; // 인덱스 검색 시 사용
		Group group;
		string current_group;
		bool previous = false;
        while (!file.eof())
        {
			pos = file.tellg();
			getline(file, str);
			if(str.front() == '#')
			{
				transform(str.begin(), str.end(),str.begin(), ::toupper);
				if(str == "#BEGIN")
				{
					continue;
				}
				else if (str == "#END")
				{
					break;
				}
			}
			if(str.front() == '[') //group
			{
				if(previous == true)
				{
					vector_group.push_back(group);
				}

				index = str.find_last_of(']');
				if(index == string::npos)
					error.append("[ERROR] Can't find ']'");
				current_group = str.substr(1,index-1);
				group = {current_group,pos,0};
				previous = true;
				continue;
			}
			else if(str.front() == ' ' || str.empty())
			{
				cout << "empty" << endl;
			}
			else if(previous == true)
			{
				group.count++;
				vector_string.push_back(str);
			}

			line++;
		}
		file.close();
	}
    return true;
}