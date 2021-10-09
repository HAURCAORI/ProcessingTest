#include "DataProcess.h"

vector<ActiveNeuron> list_neuron;
//vector<ActiveNeuron> list_threshold_neuron;

const float DELTA_TIME = 1000;//ms
const float SUSPEND_TIME = 1000;
const float EFFECTIVE_OFFSET = 0.05;

bool Load(PAGE page, SECTOR sector, Signal* signal, Neuron* previous)
{
    if(previous == nullptr)
        cout << "LOAD : " << page << "/" << sector << " from null" << endl;
    else
        cout << "LOAD : " << page << "/" << sector << " from " << previous->page << "/" << previous->sector << endl;

    FILE *stream = getPage(page)->stream;
    long pos = SectorUnit * sector;
    float value;
    float delta;

    BYTE ttype;
    ffread(stream, pos, ttype);
    if (ttype >> 7 & 1)
    {
        //----------
        // 데이터 불러오기
        //----------
        ++pos;
        NUMBER tsize;
        ffread(stream, pos, tsize);
        struct Neuron *neuron = (Neuron *)malloc(sizeof(struct Neuron) + sizeof(BYTES) * tsize);
        neuron->stream = stream;
        neuron->page = page;
        neuron->sector = sector;
        neuron->type = ttype;
        neuron->count = tsize;
        ++pos;
        long pos_temp = pos; //pos_temp = priority 위치
        pos += 2;

        ffread(stream, pos, neuron->threshold);
        pos += 4;
        ffread(stream, pos, neuron->weight);
        pos += 4;
        ffread(stream, pos, neuron->temp);

        if(previous != nullptr)
        {
            neuron->previous = previous;
        }

        //----------
        // 신호 연산
        //----------
        value = neuron->weight * (signal->value);
        delta = value - neuron->threshold;
        if (value > 1)
            value = 1;

        if (neuron->temp < value)
        {
            neuron->temp = value;
            ffwrite(stream, pos, value);
        }

        signal->value = value;

        //----------
        // ActiveNeuron 생성
        //----------
        ActiveNeuron nactive = {neuron, (float)clock() - DELTA_TIME * delta, FlagGen()};

        if (neuron->temp > neuron->threshold)
        {
            neuron->priority = UpDownData(stream, pos_temp, true);
            pos_temp++;
            neuron->effective = UpDownData(stream, pos_temp, true); //effective 증가
        }
        else
        {
            neuron->priority = UpDownData(stream, pos_temp, true);
        }

        //----------
        // List에 등록
        //----------
        list_neuron.push_back(nactive);
        float effectiveness = neuron-> effective / neuron-> priority;
        if(effectiveness > (1 - EFFECTIVE_OFFSET) || effectiveness < (1 - EFFECTIVE_OFFSET))
        {
            //cout << "effectiveness : " << effectiveness << endl; //나중에 추가
        }

        //InsertAddressAuto(neuron, 2);

        if (tsize > 0)
        {
            BYTES bytes;
            BYTES current_page = page;
            pos = SectorUnit * (neuron->sector) + 16;
            if (ttype >> 1 & 1) //branch가 true일 경우
            {
                for (int i = 0; i < tsize; i++)
                {
                    ffread(stream, pos, bytes);
                    pos += 2;
                    if (bytes == USHORT_MAX)
                    {
                        break;
                    }
                    else if (bytes == USHORT_TRA)
                    {
                        ffread(stream, pos, bytes);
                        pos += 2;
                        current_page = bytes;
                    }
                    else
                    {
                        Load(current_page, bytes, signal, nullptr);
                    }
                }
            }
            else
            {
                for (int i = 0; i < tsize; i++)
                {
                    ffread(stream, pos, bytes);
                    pos += 2;
                    if (bytes == USHORT_MAX)
                    {
                        break;
                    }
                    else if (bytes == USHORT_TRA)
                    {
                        ffread(stream, pos, bytes);
                        pos += 2;
                        current_page = bytes;
                    }
                    else
                    {
                        Load(current_page, bytes, signal, neuron);
                    }
                }
            }
        }else if(ttype && 1)//주소가 없는 neuron의 경우
        {
            if(ttype >> 1 & 1)//branch가 true일 경우
            {
                Load(page,sector+1,signal,nullptr);
            }else
            {
                Load(page,sector+1,signal,neuron);
            }
        }
        return true;
    }

    return false;
}

bool UnloadProcess()
{
    vector<ActiveNeuron>::iterator iter = list_neuron.begin();
	for (; iter != list_neuron.end(); )
	{
		if (difftime(clock(), iter->timestamp) > SUSPEND_TIME)
		{
            cout << "UNLOAD : " << iter->neuron->page << "/" << iter->neuron->sector << endl;
			UnloadNeuron(iter->neuron);
            free(iter->neuron->address);//해제해야 되는 것인지 아닌지 나중에 판단
            free(iter->neuron);
			iter = list_neuron.erase(iter);
		}
		else
		{
            break;
		}
	}
    return true;
}

void ShowProcess()
{
    vector<ActiveNeuron>::iterator iter = list_neuron.begin();
	for (; iter != list_neuron.end(); iter++)
	{
		cout<< iter->timestamp << " : " << iter->neuron->page << "|" << iter->neuron->sector << endl;
	}
}

bool UnloadNeuron(Neuron *neuron)
{
    FILE *stream = neuron->stream;
    long pos = SectorUnit * neuron->sector+11;
    float temp;
    ffread(stream, pos, temp);
    if(temp <= neuron->temp) //temp는 현재 뉴런의 temp보다 작거나 같은 경우 0으로 초기화(가장 활성도가 큰 값이 Unload되므로)
    {
        temp = 0;
        ffwrite(stream, pos, temp);
    }
    return true;
}