#include "DataProcess.h"

vector<ActiveNeuron> list_neuron;

float SuspendTime = 1000;

bool Load(PAGE page, SECTOR sector, Signal signal)
{
    FILE *stream = getPage(page)->stream;
    long pos = SectorUnit * sector;

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
            neuron->priority = UpDownData(stream, pos, true);//fread(&neuron->priority, sizeof(NUMBER), 1, stream);
            ++pos;
            neuron->extra = UpDownData(stream, pos, true);//extra 증가
            ++pos;
            ffread(stream, pos, neuron->threshold);
            pos += 4;
            ffread(stream, pos, neuron->weight);
            pos += 4;
            ffread(stream, pos, neuron->temp);
            


            ActiveNeuron nactive = {neuron,(float) clock(),FlagGen()};
            list_neuron.push_back(nactive);

            InsertAddressAuto(neuron, 2);


            if(tsize > 0)
            {
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
		if (difftime(clock(), iter->timestamp) > SuspendTime)
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
    free(stream);

    return true;
}


 //--neuron->extra; //활성된 수 줄이기 | 비정상적인 종료로 인한 오류 교정을 위한 과정 나중에 추가하기