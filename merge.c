#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {

	FILE* fp1 = NULL;
	FILE* fp2 = NULL; 
	FILE* fp3 = NULL;
	int filesize =0;
	int i=0;

	if((fp1 = fopen(argv[1], "w+")) == NULL){
		printf("fail to create file\n");
		return 1;
	}

	if((fp2 = fopen(argv[2], "r")) == NULL) {
		printf("fail to open file2\n");
		return 1;
	}

	if((fp3 = fopen(argv[3], "r")) == NULL){
		printf("fail to open file3\n");
		return 1;
	}

	//파일2의 바이트 구하기 
	fseek(fp2,  0, SEEK_END);
	filesize = ftell(fp2);
	rewind(fp2);
	//printf("FILE2 filesize : %d\n", filesize);

	//파일1에 파일2를 복사하기 
	char buffer2[filesize];	
	fread(buffer2, sizeof(char),filesize, fp2);	
	fwrite(buffer2,sizeof(char),filesize, fp1);	
	//printf("file2 : %s\n", buffer2);
	filesize =0;	

	//파일3의 바이트 구하기 
	fseek(fp3, 0, SEEK_END);
	filesize = ftell(fp3);
	rewind(fp3);
	//printf("FILE3 filesize : %d\n", filesize);

	char buffer3[filesize];
	fread(buffer3, sizeof(char), filesize, fp3);
	fwrite(buffer3, sizeof(char), filesize, fp1);
	//printf("file3 : %s\n", buffer3);

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);

	return 0;
}
