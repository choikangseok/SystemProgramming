#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#define NAMELENGTH 6 //이름의 길이를 5개로 할꺼니까 마지막엔 '\0'이 들어갈 수 있게
#define NROOMS	5//방을 5개로 설
char namebuf[NAMELENGTH];//버퍼 변수
int infile = -1;//파일을 읽기, 쓰기모드로 열기위해
off_t offset; //오프셋 변수 (위치를 조절시켜주는 변수)
char *getoccupier(int roomno) 
{
	ssize_t nread;
	if(infile == -1 &&
		 (infile = open("residents", O_RDWR)) == -1)
		//파일을 읽기, 쓰기 모드로 열어준다.
	{
		return (NULL);
	}
	offset = (roomno-1)*NAMELENGTH;
	//한줄이 넘어갈때마다 맨앞에서부터 NAMELENGTH만큼 이동하므로
	//offset 설정을 (방번호 -1)*NAMELENGTH로 해준다.

	if(lseek(infile, offset,SEEK_SET) == -1){
		//커서의 위치를 처음시작에서 부터 offset만큼 이동
		return (NULL);
	}
	if( (nread = read(infile, namebuf, NAMELENGTH)) <= 0){	
		//NAMELENGTH만큼 읽어 namebuf에 써준다.
		return (NULL);
}
	namebuf[nread - 1] = '\0'; //마지막 버퍼에 '\0'값을 넣어준다.
	return namebuf;//해당하는 방의 버퍼를 반환해준다.
}



int findfree()// 가장 작은 수의 빈방을 찾는 변수
{
	int i = 0;
	char *getoccupier(int), *p;
	for(i = 1; i <= NROOMS; i++ )                  
	{//1부터 방번호까지 검사하여 만약 문자열이 EMPTY이면 
	//해당하는 방의 번호 출력.
		if(p = getoccupier(i)){
			if(!strcmp(p,"EMPTY")){
				printf("The smallest and empty room number is %d \n ", i);
				return 0;
			}		
		}	
	}
	printf("There is no empty room\n");//EMPTY가 없을 경우.
	return 0;
	
}
int freeroom()
{
	char *getoccupier(int), *p;	
	int input;
	printf("Enter Remove Guest room number : ");
	scanf("%d", &input);
	
	if(p = getoccupier(input)){
		if(!strcmp(p,"EMPTY")){//먼저 방이 비어있는지 확인 
					//방이 비어있으면 종료시켜준다.
			printf("Room %d is already EMPTY ROOM\n", input);
			return 0;
		}
		//만약에 방이 비어있지 않으면 해당하는 (방의번호-1)*NAMELENGTH를
		//이용해 offset을 설정한다. 그리고 나서 lseek으로 커서의 위치를 
		//설정하고 해당하는 곳에 "EMPTY\n"를 써준다.(덮어써진다.)	
		offset = (input-1)*NAMELENGTH;
		lseek(infile,offset, SEEK_SET);
		write(infile, "EMPTY\n", NAMELENGTH);
	}	
}
int addguest()
{

	char *getoccupier(int), *p;
	int input;
	char Name[NAMELENGTH];

	printf("Enter add Guest room number : ");
	scanf("%d", &input);
	//비었는지 먼저 확인을합니다.
	if(p = getoccupier(input)){
		if(!strcmp(p,"EMPTY")){//만약에 비어있다면
			printf("Room %d is EMPTY ROOM\n", input);
			printf("NewGuest Name : ");//새로운 게스트를 입력받습니다.
			scanf("%s", Name);
			Name[NAMELENGTH-1]='\n';//입력을 받고 마지막 문자열을 '\n'으로 설정
			// (방의번호-1)*NAMELENGTH를 이용해 offset을 설정한다.
			// 그리고 나서 lseek으로 커서의 위치를 
			//설정하고 해당하는 곳에 Name 써준다.(덮어써진다.)	
			offset = (input-1)*NAMELENGTH;
			lseek(infile,offset, SEEK_SET);
			write(infile, Name, NAMELENGTH);
		}	
		else		//빈방이 아닐경우에 출력.
			printf("Room is not EMPTY\n");
	}
}
int main(){

 
	int j, option;
	char *getoccupier(int), *p;
 


	while(1){
		//옵션을 선택할 수 있게 합니다.
		printf("\n---Select option---\n");
		printf("1------Room Status\n");//방의 상태
		printf("2---------finefree\n");//빈방 중 가작 작은 번호의 방
		printf("3---------freeroom\n");//빈방을 만들어줍니다.
		printf("4---------addguest\n");//방에 게스트를 더합니다.
		printf("5-------------QUIT\n");//종료합니다.
		scanf("%d", &option);
		switch(option){
			case 1://1번을 누를 경우 방의 현재 상태를 나타냅니다.
			{
				for( j = 1; j <= NROOMS; j++)
				{
					if(p = getoccupier(j))
						printf("Room %2d, %s\n", j, p);
					else
						printf("Error on room %d\n", j);
				}
				break;
			}

			case 2://2번을 누를 경우  빈방 중 
				//가장 작은 번호의 방이 출력됩니다.
			{
				findfree(); break;
			}
			case 3://방을 비움니다.
			{
				freeroom(); break;
			}
			case 4://방에 게스트를 더합니다.
			{
				addguest(); break;
			}
			case 5: //종료합니다.
				return 0;
			default : return 0;
		}
	}
	close(infile);
}
