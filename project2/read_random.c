#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
//필요하면 header file 추가 가능

#define SUFFLE_NUM	10000	// 이 값은 마음대로 수정할 수 있음.

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// 필요한 함수가 있으면 더 추가할 수 있음.

int main(int argc, char **argv)
{
	int i;
	int *read_order_list;
	int num_of_records; // 레코드 파일에 저장되어 있는 전체 레코드의 수
	FILE *fp = NULL;
	char buffer[251]; //한줄 읽어서 저장할 버퍼 
	struct timeval startTime, endTime;

	memset(buffer, 0, 251); //버퍼 초기화 

	if((fp=fopen(argv[1],"rb"))==NULL){ //헤더레코드를 열기 위함 
		printf("fail to open '%s' file\n", argv[1]);
		return -1;
	}

	fread(&num_of_records, sizeof(num_of_records), 1, fp); //헤더레코드 읽음	
	printf("#records: %d ",num_of_records);

	// 이 함수를 실행하면 'read_order_list' 배열에는 읽어야 할 레코드 번호들이 나열되어 저장됨
	
	read_order_list = (int *)malloc(sizeof(int)*num_of_records); //동적할당 해줌

	GenRecordSequence(read_order_list, num_of_records); //배열을 랜덤으로 바꿔줌

	//
	// 'read_order_list'를 이용하여 random 하게 read 할 때 걸리는 전체 시간을 측정하는 코드 구현
	//

	gettimeofday(&startTime, NULL); //측정시작 

	for(i=0; i<num_of_records; i++){
		fseek( fp, 4+ (read_order_list[i]) * 250, SEEK_SET);
		fread(buffer, 250, 1, fp);
	}
	
	gettimeofday(&endTime, NULL); //측정종료 

	printf("elapsed_time: %ld us\n", (endTime.tv_sec - startTime.tv_sec)*1000000 + endTime.tv_usec - startTime.tv_usec);

	fclose(fp);

	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0)); //rand()의 값이 똑같이 나오는 것을 막기 위해서 이 문장을 씀.

	for(i=0; i<n; i++)
	{
		list[i] = i; //우선 리스트에 인덱스 차례대로 값을 저장 
	}
	
	for(i=0; i<SUFFLE_NUM; i++) //이 짓을 충분히 해줌 
	{
		j = rand() % n; //0~n-1 사이의 두 수를 임의로 뽑아서 
		k = rand() % n;
		swap(&list[j], &list[k]); //서로의 위치를 바꾸어 줌 
	}
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}
