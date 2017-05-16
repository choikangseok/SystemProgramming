#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){

        int infile, outfile, i, count =1; // count는 파일을 생성시에 사용한다. 
        int Ooption=0, Noption=0, Soption=0, Doption=0; // 해당옵션의 구분을 지을 때 사용
	int Fname, Rname; // 분할과 생성 파일명의 인자 위치 저장시 사용
        char *dir, *buffer, temp[100]; //temp는 생성 파일 문자열을 저장할 배열
        int Rsize = 524288; // 생성될 파일의 Default size 512KB는 524288바이트 이다.
	char charcnt[4];//파일생성 번호        
	ssize_t nread;
	off_t Fsize;

        for(i=1; i<argc; i++){ // argc의 수만큼 반복문을 실행합니다.
		if(!strcmp(argv[i], "-o")){ //옵션 -o일 경우
			Fname = i+1; // 대상 파일의 인수의 위치 설정합니다. 
			Ooption = 1; // -o 옵션 설정 값을 1로 설정
		}
            	if(!strcmp(argv[i], "-n")){ //-n의 문자일 경우
			Rname = i+1; // 생성될 파일의 인수의 위치 설정합니다.
			Noption = 1; // -o 옵션 설정 값을 1로 설정
		}
            	if(!strcmp(argv[i], "-s")){ //-s의 문자일 경우
			Rsize = atoi(argv[i+1])* 1024; //생성될 파일 해당 사이즈로 설정합니다. KB기준을 적용
			Soption=1; // -s 옵션 설정 값을 1로 설정
		}
		if(!strcmp(argv[i], "-d")){ //-n의 문자일 경우
			dir = argv[i+1]; // 다음 인자를 디렉토리명으로 설정합니다.
			Doption=1; // -d 옵션 설정 값을 1로 설정
			if(chdir(dir) == -1){ // -d 옵션 이후 입력 받은 디렉토리 이름이 없을 경우
                                mkdir(dir, 0777); // 입력 받은 디렉토리명으로 디렉토리 생성
			}  			  //권한을 모든 권한으로 적용
		}
        }
	if(Noption == 0){ // -n 옵션으로 입력해준 생성될 파일 명이 없다면 분할될 파일명
		printf("이름 지정 옵션(-n) : X\n");		
		Rname = Fname; // Default Name으로 사용
	}
	if(Noption == 1){ // -n 옵션이 있을 경우 
                printf("이름 지정 옵션(-n) : O\n");
        }
	if(Soption == 0)
                printf("파일크기 옵션(-s) : X\n");//-s 옵션이 없을 경우 출력
        if(Soption == 1)
                printf("파일크기 옵션(-s) : O\n");//-s 옵션이 있을 경우 출력
	if(Ooption == 0){ // -o 옵션은 필수 옵션이기 때문에 프로세스 실행 불가
		printf("-o 옵션을 사용하셔야합니다. -o 옵션을 사용하세요.\n");
                exit(1);
        }
		 printf("합칠 파일 옵션(-o) : O\n");//-o 옵션이 있을 경우 출력
	if(Doption == 0)
                printf("파일크기 옵션(-d) : X\n");//-d 옵션이 없을 경우 출력
        if(Doption == 1){
                printf("파일크기 옵션(-d) : O\n");//-d 옵션이 있을 경우 출력
		
	}
        if((infile = open(argv[Fname], O_RDONLY)) == -1){
           	fputs("InFile open Error!! \n", stdout); // 파일 열기 실패 
           	exit(1);
	}
        Fsize = lseek(infile, (off_t)0, SEEK_END); // 분할 될 파일의 크기 구함
        if((Fsize/Rsize) > 1000){ // 생성될 파일의 갯수가 1000개 이상 경우에
		printf("너무 많은 파일이 만들어 집니다. 더 큰 사이즈를 입력하세요(-s)\n");           	 
		exit(1);
        }else{
            	lseek(infile, (off_t)0, SEEK_SET); // 분할 될 파일포인터를 처음시작으로 돌려줌
        }
        //분할될, 생성될 파일의 정보 출력
	printf("분할될 파일 = %s \n", argv[Fname] );
        printf("분할될 파일 사이즈 = %fKb \n", (float)Fsize/1024);
        printf("생성될 파일 이름 = %s ... \n", argv[Rname]);
        printf("생성될 파일 사이즈 = %fKb \n", (float)Rsize/1024);

        buffer = (char *)malloc(sizeof(char)*Rsize); // 생성될 파일 크기로 
                                                            //buffer size 동적할당
	while((nread = read(infile, buffer, Rsize)) > 0){
					
		cntoa(count, charcnt);
		if(count==1000){//count가 1000일 경우에는 문자열의 수가 4개 이므로 조건을 준다.
			sprintf(temp, "%s%s", argv[Rname], "1000"); // 생성 될 파일명을 temp에 설정한다.
		}else{
			sprintf(temp, "%s%s", argv[Rname], charcnt); // 생성 될 파일명을 temp에 설정한다.
		}
		count++; // 생성될 파일의 뒤에 입력하는 숫자를 증가 시킨다.
            	chdir(dir); // 입력받은 디렉토리명으로 실행 디렉토리 변경
		//생성할 파일 열기, 쓰기전용, 파일이 없으면 생성, 있으면 크기를 0으로
            	if((outfile = open(temp, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1){
			fputs("OutFile open Error!! \n", stderr); 
                	exit(1);
            	}
            		write(outfile, buffer, nread);
           	 	close(outfile);
        }
        close(infile);
        free(buffer);
	fputs("Complete!!\n",stdout);
	
        return 0;
}

int cntoa(int pnum, char * str){ //파일이름001, 파일이름 002 이런식으로 진행을	
	*str = (pnum / 100) + 48;//하기 위해서 만들어준 함수이다.
	pnum = pnum % 100;       //해당하는 숫자를 문자열로 바꾸어준다.
	*(str+1) = (pnum / 10) + 48;
	pnum = pnum % 10;
	*(str+2) = pnum + 48;
	*(str+3) = '\0';

	return 0;
}
