#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// 필요하면 hearder file을 추가할 수 있음


int main(int argc, char **argv)
{
	//
	// 사용자로부터 입력 받은 레코드 수만큼의 레코드를 레코드 파일에 저장하는 코드를 구현
	//

	FILE *fp;
	int record_num = atoi(argv[1]); //입력받은 레코드 개수를 인트로 변환함 
	int i=0; //포문을 위한 변수
	char record[251]; //복사해줄 250byte 문자열 
	
	for(i=0; i<251; i++){
		record[i] = 'a'; //모두 a로 초기화 
	}

	if((fp=fopen(argv[2],"wb")) == NULL){ //헤더레코드를 작성하기 위함 
		return -1;
	}
	fwrite(&record_num,sizeof(record_num),1,fp); //레코드 수를 저장함
	
	for(i=0 ; i<record_num; i++){ //입력받은 레코드 개수만큼 레코드를 레코드파일에 저장함
		fwrite(record, 250, 1, fp);	
	}	

	fclose(fp);

	return 0;

}

