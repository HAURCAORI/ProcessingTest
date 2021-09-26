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

class Group
{
	private:
	string name;
	streampos pos;
	int count;
	vector<string> vector_string;
	public:
	Group()
	{
		count = 0;
	}
	Group(string name, streampos pos)
	{
		this->name = name;
		this->pos = pos;
		count = 0;
	}

	void AddItem(string str)
	{
		vector_string.push_back(str);
		count++;
	}
	vector<string> getItems(){return vector_string;}

	int getCount()
	{
		return count;
	}
	string getName(){return name;};
	void setName(string name){this->name = name;}
};

struct neuronlink
{
	string id;
	string address;
};

void Processing();
vector<Group> vector_group;
vector<string> group_name_list;

string error;

bool Mapping(){
    string address = (string)Path + "mapping";
	ifstream file(address);
	if (file.is_open())
    {
		string str;
		
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
					if(previous == true)
					{
						vector_group.push_back(group);
					}
					break;
				}
			}

			if(str.front() == ' ' || str.empty())
			{
				continue;
			}
			else if(str.front() == '[') //group
			{
				if(previous == true)
				{
					vector_group.push_back(group);
				}

				index = str.find_last_of(']');
				if(index == string::npos)
				{
					error.append("[ERROR] Can't find ']'\n");
				}
				current_group = str.substr(1,index-1);
				group_name_list.push_back(current_group);
				group = Group(current_group,pos);
				previous = true;
				continue;
			}
			else if(previous == true)
			{
				group.AddItem(str);
			}

			line++;
		}
		file.close();
		//cout << "size : " << vector_group.size() << endl;
		//cout << vector_group[1].getCount() << endl;
		Processing();
	}
	cout << error << endl;
    return true;
}

bool inGroup(string name)
{
	for(size_t i = 0; i < group_name_list.size(); i++)
	{
		if(group_name_list[i] == name)
			return true;
	}
	return false;
}

void Processing(){
	int n = vector_group.size();
	size_t index;
	for(int i = 0; i < n; i++)
	{
		Group g = vector_group[i];
		for(int j = 0; j < g.getCount(); j++) //1차적으로 neuron 목록을 만듬
		{
			string neuron_id;
			string str = g.getItems()[j];
			vector<string> div = split(str,',');
			if(div.size() == 3)
			{
				index = div[0].find_first_of('[');
				if(index == string::npos)
				{
					cout << div[0] << endl;
				}else{
					cout << div[0].substr(0,index-1) << endl;
				}
			}else{
				error.append("[WARNING] ");
				error.append(vector_group[i].getName());
				error.append("|Line:");
				error.append(to_string(j+1));
				error.append(" :: Inappropriate Count of Parameters\n");
			}
		}
		
	}
}