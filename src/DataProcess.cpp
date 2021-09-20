#include "DataProcess.h"

bool Load(PAGE page, SECTOR sector, Signal signal)
{
    FILE *stream = getPage(page)->stream;
    fseek(stream, SectorUnit * sector, SEEK_SET);
    BYTE ttype;
    fread(&ttype, sizeof(BYTE), 1, stream);
    
    if(ttype>>7 & 1)
    {
        if(!(ttype >> 6 & 1))
        {
            NUMBER tsize;
            fread(&tsize, sizeof(NUMBER), 1, stream);
            struct Neuron* neuron = (Neuron*) malloc(sizeof(struct Neuron) + sizeof(BYTES)*tsize);
            neuron->stream = stream;
            neuron->page = page;
            neuron->sector = sector;
            neuron->type = ttype;
            neuron->count = tsize;
            neuron->priority = UpDownData(stream, true);//fread(&neuron->priority, sizeof(NUMBER), 1, stream);
            neuron->extra = UpDownData(stream, true);//extra 증가
            fread(&neuron->threshold, sizeof(float), 1, stream);
            fread(&neuron->weight, sizeof(float), 1, stream);
            fread(&neuron->temp, sizeof(float), 1, stream);
            
            if(tsize > 0)
            {
                BYTES bytes[tsize];
                for(int i = 0; i < tsize; i++)
                {
                    fread(&bytes[i], sizeof(BYTES), 1, stream);
                }
            }
            return true;
        }
    }
    
    return false;
}

bool UnLoad(Neuron *neuron) //과제 : 모든 값들은 stream 내에서 즉시 수정fseek(stream, 1L, SEEK_CUR); 하도록 선언할 것
{
    FILE *stream = neuron->stream;
    fseek(stream, SectorUnit * neuron->sector, SEEK_SET);
    //--neuron->extra; //활성된 수 줄이기 | 비정상적인 종료로 인한 오류 교정을 위한 과정 나중에 추가하기
    fwrite(&neuron->type, 1, 1, stream); //type

    NUMBER tcount;
    fread(&tcount, 1,1, stream);
    if(neuron->count > tcount)
    {
        fseek(stream, -1L, SEEK_CUR); 
	    fwrite(&neuron->count, 1, 1, stream); //size
    }
	fseek(stream, 1L, SEEK_CUR); //priority 생략
	UpDownData(stream, false); //extra 감소
    fseek(stream, 8L, SEEK_CUR); // threshold와 weight는 즉시 반영하므로 생략
    float temp;
    fread(&temp, sizeof(float), 1, stream);
    if(temp == neuron->temp) //temp는 현재 뉴런의 temp와 같은 경우 0으로 초기화
    {
        temp = 0;
        fseek(stream, -4L, SEEK_CUR);
        fwrite(&temp, sizeof(float), 1, stream);
    }

}