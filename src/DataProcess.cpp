#include "DataProcess.h"

#define LOG_ELAPSED_TIME false
#define LOG_LOAD false
#define DELAY false
#define EPSILON 0.001
#include <chrono>
#include <thread>
#include <math.h>

vector<ActiveNeuron> list_neuron;
//vector<ActiveNeuron> list_threshold_neuron;

const float DELTA_TIME = 1000;//ms
const float SUSPEND_TIME = 1000;
const float EFFECTIVE_OFFSET = 0.05;
const NUMBER PRIORITY_CHECK = 10;

bool Load(PAGE page, SECTOR sector, Signal* signal, Neuron* previous)
{
    #if LOG_ELAPSED_TIME
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now(); 
    #endif

    #if LOG_LOAD
        if(previous == nullptr)
          cout << "LOAD : " << page << "/" << sector << " from null" << endl;
        else
            cout << "LOAD : " << page << "/" << sector << " from " << previous->page << "/" << previous->sector << endl;
    #endif

    FILE* &stream = getPage(page)->stream;
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
        ffread(stream, pos, tsize); // 2byte - size
        struct Neuron *neuron = (Neuron *)malloc(sizeof(struct Neuron) + sizeof(BYTES) * tsize);
        neuron->stream = stream;
        neuron->page = page;
        neuron->sector = sector;
        neuron->type = ttype;
        neuron->count = tsize;
        ++pos;
        ffread(stream, pos, neuron->specificity); // 3byte - flag
        ++pos;
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
        
        if (fabs(value) < EPSILON)
        {
            free(neuron);
            return false;
        }

        if (neuron->temp < value)
        {
            neuron->temp = value;
            ffwrite(stream, pos, value);
        }

        signal->value = value;

        cout <<  sector  << ":" <<  value << endl;

        //----------
        // ActiveNeuron 생성
        //----------
        bool valid = false;
        ActiveNeuron nactive = {neuron, (float)clock() - DELTA_TIME * delta, FlagGen()};

        if (value > neuron->threshold)
        {
            neuron->priority = UpDownData(stream, pos_temp, true);
            pos_temp++;
            neuron->effective = UpDownData(stream, pos_temp, true); //effective 증가
            neuron->is_effective = 1;
            list_neuron.push_back(nactive);
            valid = true;
        }
        else
        {
            if (neuron->temp > neuron->threshold)
            {
                neuron->is_effective = 2;
                list_neuron.push_back(nactive);
                valid = true;
            }
            else
            {
                neuron->priority = UpDownData(stream, pos_temp, true);
                neuron->is_effective = 0;
                list_neuron.push_back(nactive);
                valid = false;
            }
        }

        //----------
        // priority 및 effective 계산
        //----------

        if (neuron->priority > PRIORITY_CHECK)
        {

            float effectiveness = ((float) neuron->effective) / ((float) neuron->priority);
            if (effectiveness > (1 - EFFECTIVE_OFFSET) || effectiveness < (1 - EFFECTIVE_OFFSET))
            {
                //cout << "effectiveness : " << effectiveness << endl; //나중에 추가
            }

            pos = SectorUnit * neuron->sector + Pos_Priority;
            SetZero(stream, pos);
            pos++;
            SetZero(stream, pos);
        }
        #if DELAY
            this_thread::sleep_for(chrono::milliseconds(10));
        #endif
        //InsertAddressAuto(neuron, 2);
        if (valid)
        {
            if (tsize > 0)
            {
                BYTES bytes;
                BYTES current_page = page;

                if (ttype >> 1 & 1) //branch가 true일 경우
                {
                    long pos = SectorUnit * (neuron->sector) + NeuronHeader;
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
                    long pos = SectorUnit * (neuron->sector) + NeuronHeader;
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
            }
        }

#if LOG_ELAPSED_TIME
            std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
        #endif
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
            #if LOG_LOAD
                cout << "UNLOAD : " << iter->neuron->page << "/" << iter->neuron->sector << endl;
            #endif
			UnloadNeuron(iter->neuron);
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

bool UnloadNeuron(Neuron *neuron)
{
    FILE* &stream = neuron->stream;
    long pos = SectorUnit * neuron->sector + Pos_Priority;
    if(neuron->is_effective == 1)
    {
        neuron->priority = UpDownData(stream, pos, false);
        pos++;
        neuron->effective = UpDownData(stream, pos, false);
        //printf("[%d] priority : %d / effective : %d \n", neuron->sector, neuron->priority, neuron->effective);
    }else if(neuron->is_effective == 0){
        neuron->priority = UpDownData(stream, pos, false);
    }

    if(neuron->priority == 0) //temp는 현재 뉴런의 temp보다 작거나 같은 경우 0으로 초기화(가장 활성도가 큰 값이 Unload되므로)
    {
        pos = SectorUnit * neuron->sector + Pos_Temp;
        float temp = 0;
        ffwrite(stream, pos, temp);
    }
    return true;
}

void ShowProcess()
{
    cout << "[" << clock() << "]" << endl;
    vector<ActiveNeuron>::iterator iter = list_neuron.begin();
	for (; iter != list_neuron.end(); iter++)
	{
		cout<< iter->timestamp << " : " << iter->neuron->page << "|" << iter->neuron->sector << endl;
	}
}