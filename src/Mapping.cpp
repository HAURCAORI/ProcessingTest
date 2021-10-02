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
	int count;
	vector<string> vector_string;
	public:
	Group()
	{
		count = 0;
	}
	Group(string name)
	{
		this->name = name;
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
	SECTOR sector = USHORT_MAX;
	vector<BYTES> *bytes;
};

void Processing();
vector<Group> vector_group;
vector<string> group_name_list;// 빠른 검색을 위한 그룹 이름 지정
vector<neuronTag> neuron_list;
const vector<string> methods = {"", "x","i","o"};


string error;

bool Mapping(){
    string address = (string)Path + "mapping";
	ifstream file(address);
	if (file.is_open())
    {
		string str;
		
		int line = 1;
		size_t index; // 인덱스 검색 시 사용
		Group group;
		string current_group;
		bool previous = false;
		bool valid = false;
		bool end = false;
		//----------------------//
		//그룹 확인 및 그룹 데이터 지정
		//----------------------//
        while (!file.eof())
        {
			//pos = file.tellg();
			getline(file, str);

			//==========//
			//#태그 확인부
			//==========//
			if(str.front() == '#')
			{
				transform(str.begin(), str.end(),str.begin(), ::toupper);//대문자로 치환
				if(str == "#BEGIN"){
					valid = true;
					continue;
				}
				else if (str == "#END")
				{
					if(previous == true)
					{
						vector_group.push_back(group);
						end = true;
					}
					break;
				}
			}
			if(valid == true)
			{
				//==========//
				//line처리부
				//==========//
				if(str.front() == ' ' || str.empty()) //공백 무시
				{
					continue;
				}
				else if(str.front() == '[') //Group여부 확인
				{
					if(previous == true)
					{
						vector_group.push_back(group);
					}

					index = str.find_last_of(']'); //대괄호 종결부 확인
					if(index == string::npos)
					{
						ErrorMsg(true,"GROUP", line, "Can't find ']'.");
						valid = false;
					}
					current_group = str.substr(1,index-1);
					group_name_list.push_back(current_group);
					group = Group(current_group);
					previous = true;
					continue;
				}
				else if(previous == true)
				{
					group.AddItem(str);//Group내의 모든 데이터행을 추가
				}
				line++;
			}
		}
		file.close();
		
		if(end == false)
		{
			ErrorMsg(false,"FILE", 0, "Can't find '#END'.");
		}

		if(valid == true)
		{
			Processing(); //그룹화가 끝나면 프로세싱 진행
		}
		else
		{
			if(previous == false)
				ErrorMsg(true,"FILE", 0, "Can't find '#BEGIN'.");
			ErrorMsg(true,"FILE", 0, "Failed to mapping.");
		}
	}else{
		ErrorMsg(true,"FILE", 0, "Can't open 'mapping' file.");
		cout << error << endl;
		return false;
	}
	cout << error << endl;
    return true;
}

//++++++++++//
//함수 정의부
//++++++++++//
int elementCount(string group){ // 해당 그룹의 요소 개수 반환(필요시 나중에 변수로 저장하도록)
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

bool inMethod(string method) 
{
	for(size_t i = 0; i < methods.size(); i++)
	{
		if(methods[i] == method)
			return true;
	}
	return false;
}
//----------//
//함수 정의부
//----------//

//********************//
//  Processing()
//********************//
void Processing(){
	//----------------------//
	// i. 1차원 neuron 목록 생성
	//----------------------//
	int n = vector_group.size();
	size_t index;
	for(int i = 0; i < n; i++)//그룹의 항목 검색
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
					if(div[0].find_last_of(']') == string::npos)
					{
						ErrorMsg(false,vector_group[i].getName(), j+1, "Inappropriate ID Property. Can't find ']'.");
						continue;
					}
				}
				if(!inMethod(div[1]))
				{
					ErrorMsg(false,vector_group[i].getName(), j+1, "There is no method named '" + div[1] + "'.");
					continue;
				}
				neuronTag tag;
				tag.id = neuron_id;
				tag.group = vector_group[i].getName();
				tag.stream = div;
				neuron_list.push_back(tag);
			}else{
				ErrorMsg(false,vector_group[i].getName(), j+1, "Inappropriate Count of Parameters.");
			}
		}
	}
	//----------------------//
	// ii. 주소 개수 판단
	//----------------------//
	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		int estimate = 18; //종결자 2bytesp[0xff 0xff] + header 16bytes
		if(neuron_list[i].stream[1].empty()) //***기본 지정자***
		{
			vector<string> div = split(neuron_list[i].stream[2],'/');
			for(size_t n = 0; n < div.size(); n++)
			{
				if(div[n].find(';') != string::npos) //그룹여부는 ';' 유무로 구분
				{
					index = div[n].find_first_of(';');
					string first = div[n].substr(0,index);
					string second = div[n].substr(index+1);
					if(isnumber(first) && isnumber(second)){ //숫자쌍은 단일 주소 의미
						estimate += 6; //전환자 + 해당Page 주소 + 단일 데이터 sector 주소
					}else{
						if(inGroup(first))
						{
							if(second.empty())
							{
								//해당 그룹의 모든 데이터로 연결
								estimate += elementCount(first)*2; 
								estimate += 4; //전환자 + 해당Page 주소
							}else{
								//해당 그룹의 특수 데이터로 연결
								size_t c = std::count(second.begin(), second.end(), '|')+1;// |로 주소 구분
								estimate += c*2;
								estimate += 4; //전환자 + 해당Page 주소
							}
						}else{
							string err = first;
							estimate = 0;
							ErrorMsg(false,neuron_list[i].group, i+1, "Can't find group name '" + err + "'.");
							break;
						}
					}
				}else{
					estimate += 2; //기본 주소로 생각
				}
			}
		}else if(neuron_list[i].stream[1] == "x") //***next 지정자***
		{
			if((i+1) == neuron_list.size())
			{
				estimate = 0;
				ErrorMsg(false,neuron_list[i].group, i+1, "Can't find next neuron.");
			}else{
				estimate = 16;
			}
			
		}
		neuron_list[i].estimate = estimate;
	}
	//문법 오류가 있는 데이터 삭제
	for(vector<neuronTag>::size_type i=0; i < neuron_list.size();){ 
		if(neuron_list[i].estimate == 0){
			neuron_list.erase(neuron_list.begin() + i);
		}else
			i++;
	}

	//----------------------//
	// iii. 각 뉴런의 주소 할당
	//----------------------//
	int page = 0;
	int sector = 0;
	int countpage = 0;
	int countsector = 0;
	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		int num = (neuron_list[i].estimate / SectorUnit) + 1;
		if(neuron_list[i].estimate == 16)
		{
			int nn = (neuron_list[i+1].estimate / SectorUnit) + 1;//다음 뉴런의 정보도 고려
			if((sector + nn > USHORT_MAX-1))
			{
				page += 1;
				sector = 0;
				++countpage;
			}
			neuron_list[i].page = page;
			neuron_list[i].sector = sector;
			
			++sector;
			++countsector;;
		}
		else
		{
			if((sector + num > USHORT_MAX-1)) //65535 주소는 사용할 수 없는 주소니 할당되는 것 방지
			{
				page += 1;
				sector = 0;
				++countpage;
			}
			neuron_list[i].page = page;
			neuron_list[i].sector = sector;

			sector += num;
			countsector += num;
		}
		//cout << neuron_list[i].id << "(" << num << ") :" << neuron_list[i].page << "/" << neuron_list[i].sector << endl;
		
	}

	//----------------------//
	// iv. tag에 주소 기록
	//----------------------//
	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		vector<BYTES> temp;
		if(neuron_list[i].stream[1].empty()) //***기본 지정자***
		{
			vector<string> div = split(neuron_list[i].stream[2],'/');
			for(size_t n = 0; n < div.size(); n++)
			{
				if(div[n].find(';') != string::npos) //그룹여부는 ';' 유무로 구분
				{
					index = div[n].find_first_of(';');
					string first = div[n].substr(0,index);
					string second = div[n].substr(index+1);
					if(isnumber(first) && isnumber(second)){ //숫자쌍은 단일 주소 의미
						
					}else{
						//first가 그룹에 속해있는지는 이미 확인했으므로 생략
						if(second.empty())
						{
							//해당 그룹의 모든 데이터로 연결
								
						}else{
							//해당 그룹의 특수 데이터로 연결
							
						}
					}
				}else{
					//기본 주소로 처리
				}
			}
		}else if(neuron_list[i].stream[1] == "x") //***next 지정자***
		{
			
		}
		neuron_list[i].bytes = &temp;
	}
	



	/*
	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		cout << neuron_list[i].group << "/" << neuron_list[i].id << endl;
	}
	*/
}

void ErrorMsg(bool type, string code, int index, string message)
{
	if(type == true)
	{
		error.append("[ERROR] ");
		error.append(code);
		error.append(" | Index:");
		error.append(to_string(index));
		error.append(" :: ");
		error.append(message);
		error.append("\n");
	}
	else{
		error.append("[WARNING] ");
		error.append(code);
		error.append(" | Index:");
		error.append(to_string(index));
		error.append(" :: ");
		error.append(message);
		error.append("\n");
	}
}