#include "main.h"
#include "Calculate.h"
#include "DataProcess.h"
#include "Mapping.h"
#include <thread>

int main()
{
    if (Mapping())
    {

        Initialize();
        
        
        thread([&] {
            while (true)
            {
                InputLoad(0, random_float());
                this_thread::sleep_for(chrono::milliseconds(100)); 
            }
        }).detach();

        

        int selection;
        do
        {
            cin >> selection;
        } while (selection != 0);
    }
    else
    {
        cout << "fail....." << endl;
    }
    

    //CreateEmptyFile(0);
    //InsertDataHeader(0,0,TypeDefault());
    //InsertDataHeader(0,1,TypeDefault());
    //InsertDataHeader(0,2,TypeDefault());
    //InsertDataHeader(0,3,TypeDefault());


    /*
    
    Load(0,0,s);

    //ClearData(0,1);
    //cout << InsertAddress(0,1,0,65530) << endl;
    //ShowProcess();
    UnloadFile(0);

    SpecificDataRead(0,0);
    cout << "==" << endl;
    SpecificDataRead(0,1);
    cout << "==" << endl;
    SpecificDataRead(0,2);
    */


    
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