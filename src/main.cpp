#include "main.h"
#include "Calculate.h"
#include "DataProcess.h"

int main()
{
    LoadFile(0);
    LoadFile(1);
    //CreateEmptyFile(0);
    cout << isLoaded(0) << endl;
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