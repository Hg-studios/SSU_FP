#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "person.h"
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 위의 readPage() 함수를 호출하여 pagebuf에 저장한 후, 여기에 필요에 따라서 새로운 레코드를 저장하거나
// 삭제 레코드 관리를 위한 메타데이터를 저장합니다. 그리고 난 후 writePage() 함수를 호출하여 수정된 pagebuf를
// 레코드 파일에 저장합니다. 반드시 페이지 단위로 읽거나 써야 합니다.
//
// 주의: 데이터 페이지로부터 레코드(삭제 레코드 포함)를 읽거나 쓸 때 페이지 단위로 I/O를 처리해야 하지만,
// 헤더 레코드의 메타데이터를 저장하거나 수정하는 경우 페이지 단위로 처리하지 않고 직접 레코드 파일을 접근해서 처리한다.



//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp, 16+pagenum*PAGE_SIZE, SEEK_SET);
	fread(pagebuf,PAGE_SIZE,1,fp);
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다. 
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp, 16+pagenum*PAGE_SIZE, SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE,1, fp);
}

//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 
// 
void pack(char *recordbuf, const Person *p)
{
	// 이미 동적할당을 한 p에 내용이 담겨있음
	char s1[1] = "#";

	strcat(recordbuf, p->id);
	strcat(recordbuf, s1);
	strcat(recordbuf, p->name);
	strcat(recordbuf, s1);
	strcat(recordbuf, p->age);
	strcat(recordbuf, s1);
	strcat(recordbuf, p->addr);
	strcat(recordbuf, s1);
	strcat(recordbuf, p->phone);
	strcat(recordbuf, s1);
	strcat(recordbuf, p->email);
	strcat(recordbuf, s1);

}

// 
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다.
//
void unpack(const char *recordbuf, Person *p)
{
	memset(p,0,sizeof(Person));

	char* str = (char*)malloc(sizeof(char)*MAX_RECORD_SIZE);
	memset(str,0,sizeof(char)*MAX_RECORD_SIZE); //str 초기화	
	strcpy(str,recordbuf); //str에 복사
	
	int count=0;
	char *ptr = strtok(str, "#");
	
	
	while(ptr != NULL){
		if(count==0){
			memcpy(p->id,ptr,strlen(ptr));
			count++;
		}
		else if(count==1){
			memcpy(p->name,ptr,strlen(ptr));
			count++;
		}
		else if(count==2){
			memcpy(p->age,ptr, strlen(ptr));
			count++;
		}
		else if(count==3){
			memcpy(p->addr,ptr, strlen(ptr));
			count++;
		}
		else if(count==4){
			memcpy(p->phone,ptr, strlen(ptr));
			count++;
		}
		else if(count==5){
			memcpy(p->email,ptr, strlen(ptr));
			count++;
		}
		ptr = strtok(NULL,"#");
	}
}

//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값들을 구조체에 저장한 후 아래 함수를 호출한다.
//	
void add(FILE *fp, const Person *p)
{
	//파일에 연결되어 있는 파일포인터 fp와
	//터미널로부터 입력받은 필트값을 구조체에 저장한 p가 인자로 옴

	char* recordbuf	= malloc(sizeof(char)*MAX_RECORD_SIZE); //pack한 문자열을 담을 배열
	char* pagebuf = malloc(sizeof(char)*PAGE_SIZE); //readPage함수나 writePage함수로 넘길 배열
	char* deletebuf = malloc(sizeof(char)*PAGE_SIZE); //삭제레코드에 내용을 쓰고 나면 (delete하고 나면) 내용을 바꿔줘야하는데 그때 쓰이는 배열 
	memset(recordbuf, 0, sizeof(char)*MAX_RECORD_SIZE);
	memset(pagebuf, 0, sizeof(char)*PAGE_SIZE);
	memset(deletebuf, 0, sizeof(char)*PAGE_SIZE);

	int countPage=0; //헤더레코드에서 몇 번째 데이터 페이지인지 확인하기 위한 변수 
	int countRecord=0; //헤더레코드에서 페이지에서 몇 번째 레코드인지 확인하기 위한 변수
	int deletePage =0; //헤더레코드에서 가장 최근에 삭제된 페이지번호를 저장하기 위한 변수 
	int deleteRecord =0; //헤더레코드에서 가장 최근에 삭제된 레코드번호를 저장하기 위한 변수 
	int recordNum =0; //읽어온 pagebuf에 레코드 개수를 저장하기 위한 변수 
	int offset =0; //읽어온 pagebuf에 레코드 offset을 저장하기 위한 변수
	int length=0; //읽어온 pagebuf에 레코드 length을 저장하기 위한 변수
	int recordLen =0; //pack함수를 통해 만들어진 레코드버퍼의 길이를 위한 변수
	int nextdeletePage=0; //delete된 것에다가 쓸 때 그 recordbuf에 저장되어 있는 deletePage를 저장하기 위한 변수 
	int nextdeleteRecord=0; //delete된 것에다가 쓸 때 그 recordbuf에 저장되어 있는 deleteRecord를 저장하기 위한 변수
	int predeletePage=-1; //delete된 것에다가 쓸 때
	int predeleteRecord=-1; //delete된 것에다가 쓸 때
	int deleteoffset=0; //delete된 것에다가 쓸 때 

	//printf("add()\n");
	//printf("%s %s %s %s %s %s\n",p->id,p->name,p->age,p->addr,p->phone,p->email);

	pack(recordbuf, p); //p내용을 패킹해서 recordbuf에 받아옴
	recordLen = strlen(recordbuf);
	//printf("recordbuf의 길이는 :%d\n", recordLen);	
	//printf("recordbuf: %s\n", recordbuf);
	
	//일단 헤더레코드의 첫 번째 정수를 읽어서 데이터 페이지의 수를 구함
	fread(&countPage, sizeof(countPage), 1, fp);
	//printf("**%d\n",countPage);
	//헤더레코드의 두 번째 정수를 읽어서 레코드 파일에 존재하는 모든 레코드의 수를 구함
	fseek(fp, 4, SEEK_SET);
	fread(&countRecord, sizeof(countRecord), 1, fp);
	//printf("**%d\n",countRecord);
	
	fseek(fp, 8, SEEK_SET);
	fread(&deletePage, 4, 1, fp);
	//printf("**%d\n",deletePage);

	fseek(fp, 12, SEEK_SET);
	fread(&deleteRecord, 4, 1, fp);
	//printf("**%d\n",deleteRecord);

	if(countPage == 0){ //아직 데이터페이지가 하나도 없는 경우
		//printf("아직 데이터페이지가 하나도 없는 경우\n");
		countPage++; //페이지수 하나 증가 ->헤더레코드 첫번째정수
		countRecord++; //레코드 수 하나 증가		
		recordNum++; //해당 페이지 헤더 영역에 있는 레코드 개수 하나 증가 
		//pagebuf 헤더 영역에 첫 번째 정수, 해당 번째에 offset, length도 저장 & 레코드 내용 저장 
		memcpy(pagebuf, &recordNum, 4);
		memcpy(pagebuf+4, &offset, 4);
		memcpy(pagebuf+8, &recordLen, 4);
		memcpy(pagebuf+HEADER_AREA_SIZE, recordbuf, recordLen);		
		writePage(fp,pagebuf,0); //0번째 페이지임

		//페이지를 써주고 헤더레코드의 첫번째 정수도 바꿔줘야 함
		fseek(fp, 0, SEEK_SET);
		fwrite(&countPage, 4, 1, fp);
		//페이지를 써주고 헤더레코드의 두번째 정수도 바꿔줘야 함
		fseek(fp, 4, SEEK_SET);
		fwrite(&countRecord, 4, 1, fp);
	}

	//먼저 레코드 파일에 삭제 레코드가 존재하는지 체크해야함
	//삭제 레코드가 있다면 거기에다가 해줘야함 (countRecord랑 countPage 둘다 증가하지 않음)
	//삭제 레코드가 없다면 두 경우로 나뉨
	else if (countPage >0){ //데이터페이지가 하나 이상 있는 경우
		//printf("데이터페이지가 하나 이상 있는 경우\n");

		if(deletePage>-1 && deleteRecord==-1){ //이런 경우는 있을 수 없음 
			//printf("헤더안에 있는 delete page, record가 잘못 입력됨\n");
			return;		
		}			
		if(deletePage==-1 && deleteRecord>-1){
			//printf("헤더안에 있는 delete page, record가 잘못 입력됨\n");
			return;		
		}

		//일단 삭제레코드가 있는 경우 수행함
		//그 삭제레코드를 현재 deletePage,Record라고 하면되고
		//right size를 검사한 다음에 현재에 있는 값은 predelete가 되며
		//그 값이 가리키고 있던 값이 deletePage,Record가 됨
		//그리고 while문에 deletePage!=-1이면 while문을 돌면서 체크해줌

		if(deletePage>-1 && deleteRecord>-1){ //삭제레코드가 있는 경우
			//printf("삭제레코드가 있는 경우이고 맨 처음 삭제레코드 right size 체크하러 들어옴\n");
			//여기에서도 삭제레코드를 쓸 수 있는 경우와 없는 경우로 나뉨
			//삭제레코드를 쓸 수 없는 경우는 그냥 현재 페이지에다가 쓰는데 이 경우는 삭제레코드가 없는 경우와 같아짐
			//삭제레코드를 쓸 수 있는 경우를 다 하고 나면 return 해주기
			readPage(fp, pagebuf, deletePage);
			memcpy(&offset, pagebuf+deleteRecord*8+4,4);
			memcpy(&length, pagebuf+deleteRecord*8+8,4);
			memcpy(&nextdeletePage, pagebuf+HEADER_AREA_SIZE+offset+1, 4);
			memcpy(&nextdeleteRecord, pagebuf+HEADER_AREA_SIZE+offset+5, 4);

			if(recordLen<=length){ //이런 경우는 이 삭제레코드에 들어갈수있음
				//printf("첫번째 삭제레코드가 right size가 맞음\n");
				memcpy(pagebuf+HEADER_AREA_SIZE+offset, recordbuf, recordLen);
				writePage(fp, pagebuf, deletePage); //다시 원래의 자리에 넣어줌
				//헤더영역 수정해줘야함
				fseek(fp, 8, SEEK_SET);
				fwrite(&nextdeletePage, 4, 1, fp);
				fseek(fp, 12, SEEK_SET);
				fwrite(&nextdeleteRecord, 4, 1, fp);
				return;
			}

			while(nextdeletePage!=-1 && nextdeleteRecord!=-1){ //-1이 나올때까지 즉, 가장마지막 레코드가 나올 때까지
				//printf("첫번째 삭제레코드가 아닌 두번째이상 삭제레코드부터 어느 하나가 right size를 체크하러 들어옴\n");
				//중간에 삭제가 되었으면 return 해줌
				//삭제가 될 레코드가 있으면 pre레코드에 내가 갖고 있던 내용을 옮겨줌
				predeletePage = deletePage;
				predeleteRecord = deleteRecord;
				deletePage = nextdeletePage;
				deleteRecord = nextdeleteRecord;
				readPage(fp, pagebuf, deletePage); //현재 page를 가져옴
				memcpy(&offset, pagebuf+deleteRecord*8+4,4); //현재 record에서 offset가져옴 
				memcpy(&length, pagebuf+deleteRecord*8+8,4); //현재 record에서 length가져옴
				memcpy(&nextdeletePage, pagebuf+HEADER_AREA_SIZE+offset+1, 4); //딜맅페이지만큼 읽어옴
				memcpy(&nextdeleteRecord, pagebuf+HEADER_AREA_SIZE+offset+5, 4); //딜맅레코드만큼 읽어옴
				if(recordLen<=length){ //이런 경우는 이 삭제레코드에 들어갈수있음
					//printf("첫번째 삭제레코드가 아닌 두번째이상 삭제레코드부터 어느 하나가 right size가 맞음\n");
					//현재 page에 레코드를 써주고
					memcpy(pagebuf+HEADER_AREA_SIZE+offset, recordbuf, recordLen);
					//pre page에 내용을 next page내용으로 바꿔주기
					if(predeletePage == deletePage){ //이미 pagebuf로 읽어왔으므로 record내용만 바꿔주면 됨
						memcpy(&deleteoffset,pagebuf+8*predeleteRecord+4,4);
						memcpy(pagebuf+HEADER_AREA_SIZE+deleteoffset+1, &nextdeletePage, 4);
						memcpy(pagebuf+HEADER_AREA_SIZE+deleteoffset+5, &nextdeleteRecord, 4);
						//printf("predeletePage==deletePage, predelete:(%d,%d), nextdeletePage:(%d,%d)\n", predeletePage,predeleteRecord,nextdeletePage,nextdeleteRecord);
					}
					else { //predeletePage를 새로 page,record모두 바꿔줘야함
						readPage(fp,deletebuf, predeletePage);
						//offset을 구해서 거기로 가서 next로 내용을 바꿔줌
						memcpy(&deleteoffset, deletebuf+8*predeleteRecord+4, 4);
						memcpy(deletebuf+HEADER_AREA_SIZE+deleteoffset+1, &nextdeletePage, 4);
						memcpy(deletebuf+HEADER_AREA_SIZE+deleteoffset+5, &nextdeleteRecord, 4);
						//writePage해줘야함 
						writePage(fp, deletebuf, predeletePage);
						//printf("predeletePage!=deletePage, predelete:(%d,%d), nextdeletePage:(%d,%d)\n", predeletePage,predeleteRecord,nextdeletePage,nextdeleteRecord);
					}
					//writePage해줘야함
					writePage(fp, pagebuf, deletePage);
					//printf("(%d,%d) 레코드가 삭제됨\n", deletePage, deleteRecord);
					return; 
				}
			}
		}

		//여기로 오는 경우는 'deletePage==-1 && deleteRecord==-1' 이 경우와 삭제레코드가 있지만 쓸 수 없는 경우임
		//else if(deletePage==-1 && deleteRecord==-1){ //삭제레코드가 없는 경우 
		//}
		
		//printf("삭제레코드가 없는 경우와 삭제레코드가 있지만 쓸 수 없는 경우임\n");
		readPage(fp, pagebuf, countPage-1); //이 함수를 이용해서 해당 페이지데이터 읽어오기
		memcpy(&recordNum, pagebuf, 4); //읽어온 pagebuf에 첫 번째 4바이트를 읽어서 record 개수 저장함
		memcpy(&offset, pagebuf+8*recordNum-4, 4); //recordNum을 구하고 가장 마지막에 있는 offset구하기
		memcpy(&length, pagebuf+8*recordNum, 4); //recordNum을 구하고 가장 마지막에 있는 length구하기
		if(length < 1){ //recordLen은 1보다 작을 수 없음
			//printf("에러 : length가 1보다 작음\n");
		}
		//-> 데이터페이지 헤더영역에 남은 데이터 체크 & 데이터영역에 남은 바이트수를 가지고 체크해야함

		//1. 해당 페이지에 쓸 수 없어서 다음 페이지로 넘어가는 경우 (countRecord랑 countPage 둘다 증가함)
		if((HEADER_AREA_SIZE+offset+length+recordLen > PAGE_SIZE) || (4+8*(recordNum+1) > HEADER_AREA_SIZE)){
			//printf("삭제레코드에 쓰지 않는 경우 : 새로운 데이터페이지를 생성해야 함\n");
			//새로운 데이터페이지를 만드는 것이므로 헤더레코드도 바꿔줘야하고
			countPage++;
			countRecord++;
			fseek(fp, 0, SEEK_SET);
			fwrite(&countPage, 4, 1, fp);
			fseek(fp, 4, SEEK_SET);
			fwrite(&countRecord, 4, 1, fp);
			//데이터페이지에 새롭게 헤더영역을 작성해야함			
			memset(pagebuf, 0, sizeof(char)*PAGE_SIZE); //pagebuf를 재사용할 것이므로 버퍼를 초기화시켜줌
			recordNum=1;			
			offset=0;
			memcpy(pagebuf, &recordNum, 4);
			memcpy(pagebuf+4, &offset, 4);
			memcpy(pagebuf+8, &recordLen, 4);
			memcpy(pagebuf+HEADER_AREA_SIZE, recordbuf, strlen(recordbuf));
			writePage(fp, pagebuf, countPage-1);
				
		}

		//2. 해당 페이지에 쓸 수 있는 경우 (countRecord만 증가함)
		else{
			//printf("삭제레코드에 쓰지 않는 경우 : 원래 데이터페이지에 마저 씀\n");
			//헤더에 내용 수정				
			countRecord++;
			fseek(fp, 4, SEEK_SET);
			fwrite(&countRecord, 4 ,1, fp);
			
			//데이터페이지 내용 수정 (recordNum, offset, length, data)
			recordNum++; //레코드하나추가됨
			offset = offset+length; //offset의 값을 구해줌 
			memcpy(pagebuf, &recordNum, 4);
			memcpy(pagebuf+8*recordNum-4, &offset, 4);
			memcpy(pagebuf+8*recordNum, &recordLen, 4);
			memcpy(pagebuf+HEADER_AREA_SIZE+offset, recordbuf, strlen(recordbuf));
			writePage(fp, pagebuf, countPage-1);
		}
	}
}

//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete(FILE *fp, const char *id)
{
	//printf("delete id : %s\n",id);
	
	Person* p = (Person*)malloc(sizeof(Person)); //동적할당
	char* recordbuf	= malloc(sizeof(char)*MAX_RECORD_SIZE); //pack한 문자열을 담을 배열
	char* pagebuf = malloc(sizeof(char)*PAGE_SIZE); //readPage함수나 writePage함수로 넘길 배열

	memset(recordbuf, 0, sizeof(char)*MAX_RECORD_SIZE);
	memset(pagebuf, 0, sizeof(char)*PAGE_SIZE);
	
	int countPage=0;
	int recordNum=0;
	int deletePage=0;
	int deleteRecord=0;
	int offset=0;
	int length=0;
	char deletemark = '*';

	//모든 페이지를 읽어와서 모든 레코드를 읽고 나서 unpack을 진행하고 그 id가 매개변수와 같은 지 체크
	fseek(fp,0,SEEK_SET);	
	fread(&countPage,4,1,fp); //읽어올 페이지수를 저장
	fseek(fp,8,SEEK_SET);
	fread(&deletePage,4,1,fp); //id가 동일하여 레코드가 삭제된다면 삭제레코드에 넣을 가장 최근에 삭제된 delete Page
	fseek(fp,12,SEEK_SET);
	fread(&deleteRecord,4,1,fp); //id가 동일하여 레코드가 삭제된다면 삭제레코드에 넣을 가장 최근에 삭제된 delete Record

	for(int i=0; i<countPage; i++){
		//printf("countPage:%d\n",countPage);
		readPage(fp,pagebuf,i); //해당페이지를 버퍼로 읽어옴
		memcpy(&recordNum, pagebuf, 4);
		for(int j=0; j<recordNum; j++){ //해당페이지의 레코드수만큼
			//printf("recordNum:%d\n",recordNum);
			memcpy(&offset, pagebuf+4+8*j, 4); //해당 레코드의 offset읽기
			//printf("offset:%d\n",offset);
			memcpy(&length, pagebuf+8+8*j, 4); //해당 레코드의 length읽기
			//printf("length:%d\n",length);
			memset(recordbuf, 0, sizeof(char)*MAX_RECORD_SIZE); //버퍼에 내용 옮기기전에 초기화
			memcpy(recordbuf, pagebuf+HEADER_AREA_SIZE+offset, length); //해당 레코드의 내용을 버퍼로 옮겨옴 
			//printf("%s\n", recordbuf);
			unpack(recordbuf, p); 
			//printf("%s\n", p->id);
			if(!(strcmp(p->id,id))){ //id와 동일한 레코드를 찾은 경우
				//printf("delete() - id와 동일한 레코드를 찾았습니다. ");
				//printf("%d %d\n",i,j);
				//pagebuf 내용을 수정해준 뒤 그걸 다시 writePage해줌
				memcpy(pagebuf+HEADER_AREA_SIZE+offset, &deletemark, 1);
				memcpy(pagebuf+HEADER_AREA_SIZE+offset+1, &deletePage, 4);
				memcpy(pagebuf+HEADER_AREA_SIZE+offset+5, &deleteRecord, 4);
				writePage(fp,pagebuf,i);
				//헤더레코드의 delete 부분을 수정해줌
				fseek(fp,8,SEEK_SET);				
				fwrite(&i,4,1,fp);
				fseek(fp,12,SEEK_SET);
				fwrite(&j,4,1,fp);
				return;
			}			
		}
	}
	//printf("delete() - id와 동일한 레코드를 찾지 못했습니다.\n");
}

int main(int argc, char *argv[])
{
	FILE *fp;  // 레코드 파일의 파일 포인터

	Person *p = malloc(sizeof(Person)); //구조체 변수 선언
	
	if(!strcmp(argv[1],"a")){ //옵션 a가 왔을 때
		//printf("option a\n");
		
		if(argc != 9){
			//printf("인자가 9개가 되지 않음\n");
			return -1;
		}
		if(strlen(argv[3]) > 14){
			//printf("id가 14보다 깁니다.\n");
			return -1;		
		}
		if(strlen(argv[4]) > 18){
			//printf("name이 18보다 깁니다.\n");
			return -1;		
		}
		if(strlen(argv[5]) > 4){
			//printf("age가 4보다 깁니다.\n");
			return -1;		
		}
		if(strlen(argv[6]) > 22){
			//printf("addr가 22보다 깁니다.\n");
			return -1;		
		}
		if(strlen(argv[7]) > 16){
			//printf("phone이 16보다 깁니다.\n");
			return -1;		
		}
		if(strlen(argv[8]) > 26){
			//printf("email이 26보다 깁니다.\n");
			return -1;		
		}
		strcpy(p->id, argv[3]);
		strcpy(p->name, argv[4]);
		strcpy(p->age, argv[5]);
		strcpy(p->addr, argv[6]);
		strcpy(p->phone, argv[7]);
		strcpy(p->email, argv[8]);

		if((fp=fopen(argv[2],"r+"))==NULL){ //해당 레코드 파일이 없는 경우
			fp=fopen(argv[2],"w+"); //새로 레코드 파일을 만들어줌
			
			//파일에다가 쓰는 것이 공간을 할당 받는다는 의미인가?
			int zero =0;
			int minus=-1;
			fseek(fp,0,SEEK_SET);
			fwrite(&zero,4,1,fp);
			fseek(fp,4,SEEK_SET);
			fwrite(&zero,4,1,fp);
			fseek(fp,8,SEEK_SET);
			fwrite(&minus,4,1,fp);
			fseek(fp,12,SEEK_SET);
			fwrite(&minus,4,1,fp);
			/*
			int header[4] = {0,0,-1,-1}; //헤더레코드를 써줌
			printf("%d %d %d %d\n", header, )
			fseek(fp,0,SEEK_SET);
			fwrite()
			fwrite(header,sizeof(header),1,fp); //fwrite로 쓰면 이진정수로 저장됨
			*/
			add(fp,p);
		}
		else { //해당 레코드 파일이 있는 경우, 그 파일을 그대로 사용
			add(fp,p);
		}
	}
	else if(!strcmp(argv[1],"d")){ //옵션 d가 왔을 때

		if(argc != 4){
			//printf("d옵션의 형식이 제대로 입력되지 않음\n");
			return -1;
		}
		fp = fopen(argv[2],"r+");
		if(fp==NULL){ //해당 파일이 없음
			//printf("delete할 해당 레코드 파일이 없음\n");
			return -1;
		}
		else{ //해당 파일이 있어야 삭제할 수 있음
			delete(fp, argv[3]);
		}
	}
	else {
		//printf("terminal input error\n");
		return -1;
	}

	return 0;
}
