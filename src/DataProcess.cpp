#include "DataProcess.h"

vector<ActiveNeuron> list_neuron;

const float DELTA_TIME = 1000;//ms
const float SUSPEND_TIME = 1000;
int set_id_increment = 0;

bool Load(PAGE page, SECTOR sector, Signal signal, Neuron* previous)
{
    FILE *stream = getPage(page)->stream;
    long pos = SectorUnit * sector;
    float value;
    float delta;

    BYTE ttype;
    ffread(stream, pos, ttype);
    if(ttype>>7 & 1)
    {
        if(!(ttype >> 6 & 1))
        {
            ++pos;
            NUMBER tsize;
            ffread(stream, pos, tsize);
            struct Neuron* neuron = (Neuron*) malloc(sizeof(struct Neuron) + sizeof(BYTES)*tsize);
            neuron->stream = stream;
            neuron->page = page;
            neuron->sector = sector;
            neuron->type = ttype;
            neuron->count = tsize;
            ++pos;
            long pos_temp = pos; //temp = priority
            pos+=2;
            /*
            
            */

            ffread(stream, pos, neuron->threshold);
            pos += 4;
            ffread(stream, pos, neuron->weight);
            pos += 4;
            ffread(stream, pos, neuron->temp);
            
            //----------
            // 신호 연산
            //----------
            value = neuron->weight * signal.value;
            delta = value - neuron->threshold;
            if(value > 1)
                value = 1;

            if(neuron->temp < value)
            {
                neuron->temp = value;
                ffwrite(stream, pos, value);
            }

            ActiveNeuron nactive = {neuron,(float) clock()-DELTA_TIME*delta,FlagGen()};

            if(neuron->temp > neuron->threshold)
            {
                neuron->priority = UpDownData(stream, pos_temp, true);
                pos_temp++;
                neuron->effective = UpDownData(stream, pos_temp, true);//effective 증가

            }else{
                neuron->priority = UpDownData(stream, pos_temp, true);
            }

            list_neuron.push_back(nactive);
            
            //InsertAddressAuto(neuron, 2);


            if(tsize > 0)
            {
                //5bit true여부 확인
                //5bit는 InserAddressAuto 명령어를 수행 후 count 증가 시 &8=0일 경우 확인 후 true로 변환
                BYTES bytes[tsize];
                pos = SectorUnit*(neuron->sector) + 16;
                for(int i = 0; i < tsize; i++)
                {
                    ffread(stream, pos, bytes[i]);
                    pos += 2;
                }
            }
            return true;
        }
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
			UnloadNeuron(iter->neuron);
            free(iter->neuron);
			iter = list_neuron.erase(iter);
		}
		else
		{
			//iter++;
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

bool UnloadNeuron(Neuron *neuron) //과제 : 모든 값들은 stream 내에서 즉시 수정하도록 선언할 것
{
    FILE *stream = neuron->stream;
    long pos = SectorUnit * neuron->sector+3;
    UpDownData(stream, pos, false); //extra 감소
    pos += 8;
    float temp;
    ffread(stream, pos, temp);
    if(temp <= neuron->temp) //temp는 현재 뉴런의 temp보다 작거나 같은 경우 0으로 초기화(가장 활성도가 큰 값이 Unload되므로)
    {
        temp = 0;
        ffwrite(stream, pos, temp);
    }
    //free(stream);

    return true;
}


 //--neuron->extra; //활성된 수 줄이기 | 비정상적인 종료로 인한 오류 교정을 위한 과정 나중에 추가하기