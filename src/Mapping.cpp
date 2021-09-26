#include "Mapping.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>



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

struct neuronTag
{
	string group;
	string id;
	int estimate;
	vector<string> stream;
	PAGE page;
	SECTOR sector;
	vector<BYTE> bytes;
};

void Processing();
vector<Group> vector_group;
vector<string> group_name_list;
vector<neuronTag> neuron_list;

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

int elementCount(string group){//최적화 위해서 나중에는 변수로 이식해두기
	int count = 0;
	for(size_t i = 0; i < neuron_list.size(); i++){
		if(neuron_list[i].group == group)
		{
			count++;
		}
	}
	return count;
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
	//1차원 neuron 목록 생성
	int n = vector_group.size();
	size_t index;
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < vector_group[i].getCount(); j++)
		{
			string neuron_id;
			string str = vector_group[i].getItems()[j];
			vector<string> div = split(str,',');
			if(div.size() == 3)
			{
				index = div[0].find_first_of('[');
				if(index == string::npos)
				{
					neuron_id = div[0];
				}else{
					neuron_id = div[0].substr(0,index);
				}
				neuronTag tag;
				tag.id = neuron_id;
				tag.group = vector_group[i].getName();
				tag.stream = div;
				neuron_list.push_back(tag);
			}else{
				error.append("[WARNING] ");
				error.append(vector_group[i].getName());
				error.append("|Line:");
				error.append(to_string(j+1));
				error.append(" :: Inappropriate Count of Parameters\n");
			}
		}
	}

	// 주소 개수 판단
	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		int estimate = 18; //종결자 2bytes + header 16bytes
		if(neuron_list[i].stream[1].empty())
		{
			vector<string> div = split(neuron_list[i].stream[2],'/');
			for(size_t n = 0; n < div.size(); n++)
			{
				if(div[n].find(';') != string::npos)
				{
					index = div[n].find_first_of(';');
					string first = div[n].substr(0,index);
					string second = div[n].substr(index+1);
					if(isnumber(first) && isnumber(second)){
						estimate += 4;
					}else{
						if(inGroup(first))
						{
							if(second.empty())
							{
								estimate += elementCount(first)*2;
								estimate += 2; //전환자
							}else{
								size_t c = std::count(second.begin(), second.end(), '|')+1;
								estimate += c*2;
								estimate += 2; //전환자
							}
						}else{
							error.append("[ERROR] [");
							error.append(neuron_list[i].group);
							error.append("] :: Can't Find group name '");
							error.append(first);
							error.append("'\n");
						}
					}
				}else{
					estimate += 2;
				}
			}
		}

		neuron_list[i].estimate = estimate;
	}

	// 각 뉴런의 주소 할당
	int page = 0;
	int sector = 0;
	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		int n = (neuron_list[i].estimate / SectorUnit) + 1;
		if((sector + n > USHORT_MAX-1)) //65535 주소는 사용할 수 없는 주소니 할당되는 것 방지
		{
			page += 1;
			sector = 0;
		}

		neuron_list[i].page = page;
		neuron_list[i].sector = sector;

		sector += n;
		
		//cout << neuron_list[i].id << "(" << n <<  ") :" << neuron_list[i].page << "/" << neuron_list[i].sector << endl;
	}
	
	// 파일에 직접 기록
	

	/*
	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		cout << neuron_list[i].group << "/" << neuron_list[i].id << endl;
	}
	*/
}