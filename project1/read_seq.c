#include <stdio.h>
#include <string.h>
#include <sys/time.h>

//필요하면 header file 추가 가능

int main(int argc, char **argv)
{
	//
	// 레코드 파일로부터 전체 레코들을 순차적으로 read할 때
	// 걸리는 전체 시간을 측정하는 코드 구현
	//

	char buffer[251]; //한줄을 읽어서 저장할 버퍼
	int num =0; //레코드 개수가 적힌 레코드 헤더를 읽기 위한 파일포인터 
	FILE* fp; //레코드 파일의 헤더 레코드를 읽기 위한 파일포인터 
	struct timeval startTime, endTime; //수행시간을 측정하기 위한 구조체 

	memset(buffer, 0, 251); //초기화 

	if( (fp = fopen(argv[1], "rb")) == NULL) {
		printf("fail to open '%s' file!\n", argv[1]);
		return -1;
	}

	fread(&num,sizeof(num),1,fp); //헤더부분에 레코드 개수를 가지고 옴 
	printf("#records: %d ",num);
	

	gettimeofday(&startTime, NULL); //측정시작 

	for(int i=0; i<num; i++){
		fread(buffer, 250, 1, fp); //헤더부분은 앞에서 읽었으니 그 뒷부분부터 레코드 크기인 250만큼씩 가져옴
	}

	gettimeofday(&endTime, NULL); //측정종료

	printf("elapsed_time: %ld us\n", (endTime.tv_sec - startTime.tv_sec)*
			1000000 + endTime.tv_usec - startTime.tv_usec);

	fclose(fp);

	return 0;
}
