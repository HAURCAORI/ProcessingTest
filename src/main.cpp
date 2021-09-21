#include "main.h"
#include "Calculate.h"
#include "DataProcess.h"

int main()
{
    //CreateEmptyFile(0);
    //InsertDataHeader(0,0,TypeDefault());
    //InsertDataHeader(0,1,TypeDefault());
    //InsertDataHeader(0,2,TypeDefault());
    InsertDataHeader(0,3,TypeDefault());
    Signal s = SignalGen(1.0);
    Load(0,0,s);
    Load(0,1,s);
    Load(0,2,s);
    Load(0,3,s);
    test();
    //ClearData(0,1);
    //cout << InsertAddress(0,1,0,65530) << endl;
    ShowProcess();
/*
    SpecificDataRead(0,0);
    cout << "==" << endl;
    SpecificDataRead(0,1);
    cout << "==" << endl;
    SpecificDataRead(0,2);
*/


    UnloadFile(0);
    /*
	FILE* pFile = fopen("resource/sample.jpg", "r"); //read mode 
    if(pFile == NULL)
    {
        printf("파일이 없습니다. 프로그램을 종료합니다.");
        return 0;

    }
	
	char str[50];
    fgets(str, 50, pFile);  //파일에 있는거 읽어오기
    
    fclose(pFile);          //파일 닫기
    printf("%s\n", str);    //터미널에 출력

*/
	return 0;
}