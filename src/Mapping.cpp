#include "Mapping.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>

#define LOG false


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
	vector<BYTES> bytes;
};

struct AddressSet
{
	vector<BYTES> bytes;
	PAGE page;
};

struct GroupSet
{
	string group;
	vector<neuronTag> list;
};

void Processing(const PAGE offset_page, const SECTOR offset_sector);
vector<Group> vector_group;
vector<string> group_name_list;// 빠른 검색을 위한 그룹 이름 지정
vector<neuronTag> neuron_list;
vector<AddressSet> address_list;
vector<GroupSet> groupset_list;
const vector<string> methods = {"", "x","i","o"};


string error;

bool Mapping(){
	vector_group.clear();
	group_name_list.clear();
	neuron_list.clear();//포인터 해제
	address_list.clear();
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
		PAGE offset_page = 0;
		SECTOR offset_sector = 0;
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
					line++;
					continue;
				}
				else if (str == "#END")
				{
					end = true;
					if(previous == true)
					{
						vector_group.push_back(group);
					}
					break;
				}
				else if (str.substr(0,5) =="#PAGE")
				{
					string temp = str.substr(str.find_first_of('=')+1);
					if(isnumber(temp))
					{
						if(stoi(temp) <= 65535 && stoi(temp) >= 0)
						{
							offset_page = stoi(temp);
						}else{
							ErrorMsg(true,"FILE", line, "Value of '#PAGE' must be between 0 and 65535.");
							break;
						}
					}
					else
					{
						ErrorMsg(false,"FILE", line, "Value of '#PAGE' is not a number");
					}
					line++;
					continue;
				}
				else if (str.substr(0,7) =="#SECTOR")
				{
					string temp = str.substr(str.find_first_of('=')+1);
					if(isnumber(temp))
					{
						if(stoi(temp) <= 65535 && stoi(temp) >= 0)
						{
							offset_sector = stoi(temp);
						}else{
							ErrorMsg(true,"FILE", line, "Value of '#SECTOR' must be between 0 and 65535.");
							break;
						}
					}
					else
					{
						ErrorMsg(false,"FILE", line, "Value of '#SECTOR' is not a number");
					}
					line++;
					continue;
				}
			}
			if(valid == true)
			{
				//==========//
				//line처리부
				//==========//
				if(str.front() == ' ' || str.empty()) //공백 무시
				{
					line++;
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

					for(size_t i = 0; i < group_name_list.size(); i++)//이름 같은지 확인
					{
						if(group_name_list[i] == current_group)
						{
							ErrorMsg(true,"GROUP", line, "Group name must be different.");
							valid = false;
							break;
						}
					}
					
					group_name_list.push_back(current_group);
					group = Group(current_group);

					previous = true;
					line++;
					continue;
				}
				else if(previous == true)
				{
					str.erase(remove(str.begin(), str.end(), ' '), str.end());
					group.AddItem(str);//Group내의 모든 데이터행을 추가
				}
			}
			line++;
		}
		file.close();
		
		if(end == false)
		{
			ErrorMsg(false,"FILE", 0, "Can't find '#END'.");
		}

		if(valid == true)
		{
			Processing(offset_page,offset_sector); //그룹화가 끝나면 프로세싱 진행
		}
		else
		{
			if(previous == false)
				ErrorMsg(true,"FILE", 0, "Can't find '#BEGIN'.");
			ErrorMsg(true,"FILE", 0, "Failed to mapping.");
		}
	}else{
		ErrorMsg(true,"FILE", 0, "Can't open 'mapping' file.");
		std::cout << error << endl;
		return false;
	}
	std::cout << error << endl;
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

void CreateAddressSet(const PAGE countpage, const PAGE offset_page)
{
	address_list.clear();
	for(int i = 0; i < (countpage+1);i++)
	{
		AddressSet temp;
		temp.page = (offset_page+i);
		address_list.push_back(temp);
	}
}

void AddAddressSet(neuronTag tag)
{
	for(size_t i = 0; i < address_list.size(); i++)
	{
		if(address_list[i].page == tag.page)
		{
			address_list[i].bytes.push_back(tag.sector);
			break;
		}
	}
}

bool AssignGroupSet()
{
	for(size_t i = 0; i < group_name_list.size(); i++)//목록 생성
	{
		GroupSet temp;
		temp.group = group_name_list[i];
		groupset_list.push_back(temp);
	}
	for(size_t i = 0; i < neuron_list.size(); i++)//목록에 값 기록
	{
		for(size_t j = 0 ; j < group_name_list.size(); j++)
		{
			if(neuron_list[i].group == group_name_list[j])
			{
				bool valid = true;
				for(size_t k = 0; k < groupset_list[j].list.size(); k++)
				{
					if(groupset_list[j].list[k].id==neuron_list[i].id)
					{
						valid = false;
						break;
					}
					
				}
				if(valid)
					groupset_list[j].list.push_back(neuron_list[i]);
				else
				{
					ErrorMsg(true,"GROUP", 0, "There are same id '" + neuron_list[i].id + "' in Group '" + neuron_list[i].group +  "'.");
					return false;
				}
				break;
			}
		}
	}
	return true;
}
//----------//
//함수 정의부
//----------//

//********************//
//  Processing()
//********************//
void Processing(const PAGE offset_page, const SECTOR offset_sector){
	//----------------------//
	// i. 1차원 neuron 목록 생성
	//----------------------//
	bool valid = true;
	size_t index;
	for(size_t i = 0; i < vector_group.size(); i++)//그룹의 항목 검색
	{
		if(vector_group[i].getName() == "INPUT")
		{
			for(int j = 0; j < vector_group[i].getCount(); j++)
			{
				string neuron_id;
				string str = vector_group[i].getItems()[j];
				if(str.find("->") != string::npos)
				{
					vector<string> div = split(str,'>');
					if(div.size() == 2)
					{
						index = div[0].find_first_of('[');
						if(index == string::npos)
						{
							neuron_id = div[0];
						}else{
							neuron_id = div[0].substr(0,index);
							if(div[0].find_last_of(']') == string::npos)
							{
								ErrorMsg(true,vector_group[i].getName(), j+1, "Inappropriate ID Property. Can't find ']'.");
								valid = false;
								break;
							}
						}
						neuronTag tag;
						tag.id = neuron_id;
						tag.group = "INPUT";
						tag.stream = div;
						neuron_list.push_back(tag);
					}
				}
				else
				{
					ErrorMsg(true,vector_group[i].getName(), j+1, "INPUT must be contain '->' symbol.");
					valid = false;
					break;
				}
			}
		}
		else if(vector_group[i].getName() == "OUTPUT")
		{
			for(int j = 0; j < vector_group[i].getCount(); j++)
			{
				string neuron_id;
				string str = vector_group[i].getItems()[j];
				vector<string> div;
				div.push_back(str);
				div.push_back(str);
				index = str.find_first_of('[');
				if(index == string::npos)
				{
					neuron_id = str;
				}else{
					neuron_id = str.substr(0,index);
					if(str.find_last_of(']') == string::npos)
					{
						ErrorMsg(true,vector_group[i].getName(), j+1, "Inappropriate ID Property. Can't find ']'.");
						valid = false;
						break;
					}
				}
				neuronTag tag;
				tag.id = neuron_id;
				tag.group = "OUTPUT";
				tag.stream = div;
				neuron_list.push_back(tag);
			}
		}
		else
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
							ErrorMsg(true,vector_group[i].getName(), j+1, "Inappropriate ID Property. Can't find ']'.");
							valid = false;
							break;
						}
					}
					if(!inMethod(div[1]))
					{
						ErrorMsg(true,vector_group[i].getName(), j+1, "There is no method named '" + div[1] + "'.");
						valid = false;
						break;
					}
					neuronTag tag;
					tag.id = neuron_id;
					tag.group = vector_group[i].getName();
					tag.stream = div;
					neuron_list.push_back(tag);
				}else{
					ErrorMsg(true,vector_group[i].getName(), j+1, "Inappropriate Count of Parameters.");
					valid = false;
					break;
				}
			}
		}
	}

	if(valid == false)
	{
		ErrorMsg(true,"FILE", 0, "Failed to mapping.");
		return;
	}
	//----------------------//
	// ii. 주소 개수 판단
	//----------------------//
	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		int estimate = 18; //종결자 2bytesp[0xff 0xff] + header 16bytes
		if(neuron_list[i].group == "INPUT")
		{
			vector<string> div = split(neuron_list[i].stream[1],'/');
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
								estimate += 4; //전환자 + 해당Page 주소 나중에 수정 page가 달라지는 여부는 다르게 판단
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
					estimate = 0;
					ErrorMsg(false,neuron_list[i].group, i+1, "INPUT target address should include PAGE.");
					break;
				}
			}
			neuron_list[i].estimate = estimate;
			continue;
		}
		else if(neuron_list[i].group == "OUTPUT")
		{
			neuron_list[i].estimate = 31;
			continue;
		}
		else if(neuron_list[i].stream[1].empty()) //***기본 지정자***
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
								estimate += 4; //전환자 + 해당Page 주소 나중에 수정 page가 달라지는 여부는 다르게 판단
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
	//문법 오류가 있는 데이터 삭제 warning으로 진행 가능
	for(vector<neuronTag>::size_type i=0; i < neuron_list.size();){ 
		if(neuron_list[i].estimate == 0){
			neuron_list.erase(neuron_list.begin() + i);
		}else
			i++;
	}

	//----------------------//
	// iii. 각 뉴런의 주소 할당
	//----------------------//
	int page = 0 + offset_page;
	int sector = 0 + offset_sector;
	int sector_input = 0;
	int sector_output = 0;
	int countpage = 0;
	int countsector = 0;
	bool previous = false;
	for(size_t i = 0; i < neuron_list.size(); i++)
	{
		int num = (neuron_list[i].estimate / SectorUnit) + 1;
		if(neuron_list[i].group != "INPUT" && neuron_list[i].group != "OUTPUT"){
		if(neuron_list[i].estimate == 16)
		{
			num = 1;
			int nn = (neuron_list[i+1].estimate / SectorUnit) + 1;//다음 뉴런의 정보도 고려
			if((sector + num + nn > USHORT_MAX-2))
			{
				page += 1;
				sector = 0;
				if(previous == true)
				{
					++countpage;
				}
			}
			neuron_list[i].page = page;
			neuron_list[i].sector = sector;
			previous = true;

			++sector;
			++countsector;;
		}
		else
		{
			if((sector + num > USHORT_MAX-2)) //65535 주소는 사용할 수 없는 주소니 할당되는 것 방지
			{
				page += 1;
				sector = 0;
				if(previous == true)
				{
					++countpage;
				}
			}
			neuron_list[i].page = page;
			neuron_list[i].sector = sector;
			previous = true;

			sector += num;
			countsector += num;
		}
		}
		else if(neuron_list[i].group == "INPUT")
		{
			neuron_list[i].page = USHORT_MAX;
			neuron_list[i].sector = sector_input;
			sector_input += num;
		}
		else if(neuron_list[i].group == "OUTPUT")
		{
			neuron_list[i].page = USHORT_MAX;
			neuron_list[i].sector = sector_output;
			sector_output += 2;
		}
		if(LOG)
			cout << neuron_list[i].id << "(" << num << ") :" << neuron_list[i].page << "/" << neuron_list[i].sector << endl;
	}

	//----------------------//
	// iv. tag에 주소 기록
	//----------------------//
	if(!AssignGroupSet())
	{
		ErrorMsg(true,"FILE", 0, "Failed to mapping.");
		return;
	}

	for(size_t g = 0; g < groupset_list.size(); g++)
	{
		if(groupset_list[g].group != "INPUT" && groupset_list[g].group != "OUTPUT")
		{
		for(size_t i = 0; i < groupset_list[g].list.size(); i++)
		{
			CreateAddressSet(countpage, offset_page);
			if(groupset_list[g].list[i].stream[1].empty()) // 기본 생성자
			{
				vector<string> div = split(groupset_list[g].list[i].stream[2],'/');
				for(size_t n = 0; n < div.size(); n++)
				{
					if(div[n].empty())
						continue;

					if(div[n].find(';') != string::npos) //그룹여부는 ';' 유무로 구분
					{
						index = div[n].find_first_of(';');
						string first = div[n].substr(0,index);
						string second = div[n].substr(index+1);
						if(isnumber(first) && isnumber(second)){ //숫자쌍은 단일 주소 의미
							bool check = false;
							int temp_p = stoi(first);
							int temp_s = stoi(second);
							if(!(temp_p <= 65535 && temp_p >= 0))
							{
								ErrorMsg(true,groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "Value of page must be between 0 and 65535.");
								valid = false;
							}
							if(!(temp_s <= 65535 && temp_s >= 0))
							{
								ErrorMsg(true,groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "Value of sector must be between 0 and 65535.");
								valid = false;
							}
							for(size_t al = 0; al < address_list.size(); al++)
							{
								if(address_list[al].page == temp_p)
								{
									address_list[al].bytes.push_back(temp_s);
									check = true;
									break;
								}
							}
							if(!check)
							{
								AddressSet temp;
								temp.page = temp_p;
								temp.bytes.push_back(temp_s);
								address_list.push_back(temp);
							}
						}else{
							//first가 그룹에 속해있는지는 이미 확인했으므로 생략
							if(second.empty())
							{
								//해당 그룹의 모든 데이터로 연결
								if(first != groupset_list[g].list[i].group)//자기 자신의 그룹이 아니면
								{
									for(size_t tg = 0; tg < group_name_list.size(); tg++)
									{
										if(group_name_list[tg] == first)
										{
											for(size_t t = 0; t < groupset_list[tg].list.size(); t++)
											{
												AddAddressSet(groupset_list[tg].list[t]);
											}
										}
									}
								}
								else
								{
									ErrorMsg(true, groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "Try to self-reference group.");
									valid = false;
								}
							}else{
								//해당 그룹의 특수 데이터로 연결	
								if(first != groupset_list[g].list[i].group)//자기 자신의 그룹이 아니면
								{
									for(size_t tg = 0; tg < group_name_list.size(); tg++)
									{
										if(group_name_list[tg] == first)
										{
											vector<string> element = split(second,'|');
											for(size_t te = 0; te < element.size(); te++)
											{
												bool exist = false;
												for(size_t t = 0; t < groupset_list[tg].list.size(); t++)
												{
													if(groupset_list[tg].list[t].id==element[te])
													{
														AddAddressSet(groupset_list[tg].list[t]);
														exist = true;
														break;
													}
												}
												if(!exist)
												{
													ErrorMsg(true, groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "There are no name of '"+ element[te] + "'.");
													valid = false;
												}
											}
										}
									}
								}
								else
								{
									ErrorMsg(true, groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "Try to self-reference group.");
									valid = false;
								}
							}
						}
					}else{
						//기본 주소로 처리
						bool check = false;
						for(size_t t = 0; t < groupset_list[g].list.size(); t++)
						{
							if(groupset_list[g].list[t].id == div[n])
							{
								check = true;
								if(i==t)
								{
									ErrorMsg(true, groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "Try to self-reference id.");
									valid = false;	
									break;
								}
								else
								{
									AddAddressSet(groupset_list[g].list[t]);
									break;
								}
							}
						}
						if(!check)
						{
							ErrorMsg(true, groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "There is no name of '" + div[n] + "'.");
							valid = false;
						}
					}
				}
			}
			

			if(LOG){
				cout << "[" << groupset_list[g].list[i].group << ":" << groupset_list[g].list[i].id << "]" << "estimate : " << groupset_list[g].list[i].estimate-16 << endl;
				for(size_t a = 0; a < address_list.size(); a++)
				{
					for(size_t b = 0; b < address_list[a].bytes.size(); b++)
					{
						cout << "page : " << address_list[a].page << " / sector : " << address_list[a].bytes[b] << endl;
					}
				}
			}

			for(size_t a = 0 ; a < address_list.size(); a++)
			{
				if(groupset_list[g].list[i].page == address_list[a].page)
				{
					for(size_t b = 0; b < address_list[a].bytes.size(); b++)
					{
						groupset_list[g].list[i].bytes.push_back(address_list[a].bytes[b]);
					}
				}else{
					if(address_list[a].bytes.size()>0)
					{
						groupset_list[g].list[i].bytes.push_back(USHORT_TRA);
						groupset_list[g].list[i].bytes.push_back(address_list[a].page);
					}
					for(size_t b = 0; b < address_list[a].bytes.size(); b++)
					{
						groupset_list[g].list[i].bytes.push_back(address_list[a].bytes[b]);
					}
				}
			}
			if(groupset_list[g].list[i].stream[1] != "x")
			{
				groupset_list[g].list[i].bytes.push_back(USHORT_MAX);
			}

			/*
			for(size_t l=0; l < groupset_list[g].list[i].bytes.size(); l++)
			{
				cout << groupset_list[g].list[i].bytes[l] << ";";
			}
			cout << endl;
			*/
			
		}
		}
		else if(groupset_list[g].group == "INPUT")
		{
			for(size_t i = 0; i < groupset_list[g].list.size(); i++)
			{
				CreateAddressSet(countpage, offset_page);
				vector<string> div = split(groupset_list[g].list[i].stream[1],'/');
				for(size_t n = 0; n < div.size(); n++)
				{
					if(div[n].empty())
						continue;

					index = div[n].find_first_of(';');
					string first = div[n].substr(0,index);
					string second = div[n].substr(index+1);
					if(isnumber(first) && isnumber(second)){ //숫자쌍은 단일 주소 의미
						bool check = false;
						int temp_p = stoi(first);
						int temp_s = stoi(second);
						if(!(temp_p <= 65535 && temp_p >= 0))
						{
							ErrorMsg(true,groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "Value of page must be between 0 and 65535.");
							valid = false;
						}
						if(!(temp_s <= 65535 && temp_s >= 0))
						{
							ErrorMsg(true,groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "Value of sector must be between 0 and 65535.");
							valid = false;
						}
						for(size_t al = 0; al < address_list.size(); al++)
						{
							if(address_list[al].page == temp_p)
							{
								address_list[al].bytes.push_back(temp_s);
								check = true;
								break;
							}
						}
						if(!check)
						{
							AddressSet temp;
							temp.page = temp_p;
							temp.bytes.push_back(temp_s);
							address_list.push_back(temp);
						}
					}else{
						if(second.empty())
						{
							//해당 그룹의 모든 데이터로 연결
							if(first != groupset_list[g].list[i].group)//자기 자신의 그룹이 아니면
							{
								for(size_t tg = 0; tg < group_name_list.size(); tg++)
								{
									if(group_name_list[tg] == first)
									{
										for(size_t t = 0; t < groupset_list[tg].list.size(); t++)
										{
											AddAddressSet(groupset_list[tg].list[t]);
										}
									}
								}
							}
							else
							{
								ErrorMsg(true, groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "Try to self-reference group.");
								valid = false;
							}
						}else{
							//해당 그룹의 특수 데이터로 연결	
							if(first != groupset_list[g].list[i].group)//자기 자신의 그룹이 아니면
							{
								for(size_t tg = 0; tg < group_name_list.size(); tg++)
								{
									if(group_name_list[tg] == first)
									{
										vector<string> element = split(second,'|');
										for(size_t te = 0; te < element.size(); te++)
										{
											bool exist = false;
											for(size_t t = 0; t < groupset_list[tg].list.size(); t++)
											{
												if(groupset_list[tg].list[t].id==element[te])
												{
													AddAddressSet(groupset_list[tg].list[t]);
													exist = true;
													break;
												}
											}
											if(!exist)
											{
												ErrorMsg(true, groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "There are no name of '"+ element[te] + "'.");
												valid = false;
											}
										}
									}
								}
							}
							else
							{
								ErrorMsg(true, groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, n+1, "Try to self-reference group.");
								valid = false;
							}
						}
					}
				}
			

				if(LOG){
					cout << "[" << groupset_list[g].list[i].group << ":" << groupset_list[g].list[i].id << "]" << "estimate : " << groupset_list[g].list[i].estimate-16 << endl;
					for(size_t a = 0; a < address_list.size(); a++)
					{
						for(size_t b = 0; b < address_list[a].bytes.size(); b++)
						{
							cout << "page : " << address_list[a].page << " / sector : " << address_list[a].bytes[b] << endl;
						}
					}
				}

				for(size_t a = 0 ; a < address_list.size(); a++)
				{
					if(address_list[a].bytes.size()>0)
					{
						groupset_list[g].list[i].bytes.push_back(USHORT_TRA);
						groupset_list[g].list[i].bytes.push_back(address_list[a].page);
					}
					for(size_t b = 0; b < address_list[a].bytes.size(); b++)
					{
						groupset_list[g].list[i].bytes.push_back(address_list[a].bytes[b]);
					}
				}

			/*
			for(size_t l=0; l < groupset_list[g].list[i].bytes.size(); l++)
			{
				cout << groupset_list[g].list[i].bytes[l] << ";";
			}
			cout << endl;
			*/
			}
		}
	}


	//----------------------//
	// v. 파일에 데이터 기록
	//----------------------//
	for(PAGE i = 0; i <= countpage; i++)
	{
		CreateEmptyFile(i+offset_page);
	}
	for(size_t g = 0; g < groupset_list.size(); g++)
	{
		for(size_t i = 0; i < groupset_list[g].list.size(); i++)
		{
			PAGE p = groupset_list[g].list[i].page;
			SECTOR s = groupset_list[g].list[i].sector;
			

			size_t index1 = groupset_list[g].list[i].stream[0].find_first_of('[');
			size_t index2 = groupset_list[g].list[i].stream[0].find_first_of(']');
			if(index1 == string::npos || index2 == string::npos)
			{
				InsertDataHeader(p,s,TypeDefault());
			}else{
				string temp = groupset_list[g].list[i].stream[0].substr(index1+1, index2-index1-1);
				if(groupset_list[g].list[i].stream[0].find_first_of('|') == string::npos)
				{
					InsertDataHeader(p,s,TypeDefault());
					ErrorMsg(false, groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, 0, "Wrong ID property.");
				}else{
					vector<string> value = split(temp,'|');
					if(value.size() == 2)
					{
						float threshold = stof(value[0]);
						float weight = stof(value[1]);
						InsertDataHeader(p,s,TypeDefault(),threshold,weight);
					}
					else
					{
						InsertDataHeader(p,s,TypeDefault());
						ErrorMsg(false, groupset_list[g].list[i].group + ":" + groupset_list[g].list[i].id, 0, "Wrong ID property.");
					}
					
				}
			}
			
			string address = (string)Path + to_string(p);
			FILE *stream = fopen(address.c_str(), "r+");
			if(stream) {
				long pos = SectorUnit * s + 16;
				for(size_t a = 0; a < groupset_list[g].list[i].bytes.size(); a++)
				{
					ffwrite(stream,pos, groupset_list[g].list[i].bytes[a]);
					pos+=2;
				}
			}else
			{
				ErrorMsg(true,"FILE", 0, "Failed to open data file.");
			}
			
			fclose(stream);
		}
	}

	if(valid == false)
	{
		ErrorMsg(true,"FILE", 0, "Failed to mapping.");
		return;
	}

	cout << "SUCCESS" << endl;
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
}	/*
	for(size_t i = 0 ; i < groupset_list.size(); i++)
	{
		cout << "[" << groupset_list[i].group << "]" << endl;
		for(size_t j = 0; j < groupset_list[i].list.size(); j++)
		{
			cout << groupset_list[i].list[j].id << endl;
		}
	}
	*/