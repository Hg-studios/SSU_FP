#include <stdio.h>
#include <string.h>

int main (int argc, char* argv[]){

	FILE* fp1 = fopen(argv[1],"r");
	FILE* fp2 = fopen(argv[2],"w+");

	char buffer[11] = { 0, };
	long filesize =0;
	long line=0;
	int i =0;

	if(fp1 == NULL){
		printf("fail to open origin file\n");
		return 1;
	}

	if(fp2 == NULL){
		printf("fail to create copy file\n");
		return 1;
	}

	//파일전체 바이트 구하기 
	fseek(fp1, 0, SEEK_END);
	filesize = ftell(fp1);
	rewind(fp1);

	//10바이트씩 잘랐을 때 나오는 개수 구하기 
	line = filesize/10;

	//10바이트씩 copy하기 
	for(i=0; i<line; i++){
		fread(buffer, sizeof(char), 10, fp1);
		fwrite(buffer, sizeof(char), 10, fp2);
		//printf("10개씩: %s\n",buffer);
		memset(buffer, 0, 11);
	}

	//10바이트가 되지 않는 나머지 데이터를 copy하기 
	fread(buffer, sizeof(char), filesize-10*line, fp1);
	fwrite(buffer, sizeof(char), filesize-10*line, fp2);
	//printf("나머지: %s", buffer);
	memset(buffer, 0, 11);
	
	fclose(fp1);
	fclose(fp2);

	return 0;
}
