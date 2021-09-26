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
	int size = 16;
	vector<string> stream;
	vector<string> address;
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
	int count;
	for(int i = 0; i < neuron_list.size(); i++){
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
	int n = vector_group.size();
	size_t index;
	for(int i = 0; i < n; i++) //Tag 만듬
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
	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		neuronTag tag = neuron_list[i];
		if(tag.stream[1].empty())
		{
			vector<string> div = split(tag.stream[2],'/');
			int estimate = 0;
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
								estimate += 2;
							}else{
								size_t c = std::count(second.begin(), second.end(), '|')+1;
								estimate += c*2;
								estimate += 2;
							}
						}else{
							error.append("[ERROR] [");
							error.append(tag.group);
							error.append("] :: Can't Find group name '");
							error.append(first);
							error.append("'\n");
						}
					}
				}else{
					
				}
			}
		}
	}

	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		cout << neuron_list[i].group << "/" << neuron_list[i].id << endl;
	}
}