#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 512 //버퍼사이즈

main(int argc, char * argv[]){

        int infile, outfile, final_p=0, start_p=0, i, j=0; 
	char *SetFileName, *TempFileName; 
        int Ooption=0, Xoption=0, Noption=0, Doption=0; // 해당 옵션 구별에 사용
	char *buffer, *dir;
        ssize_t nread,out_t;
	char PresentDir[SIZE];
	
	if(getcwd(PresentDir, SIZE) == NULL){//프로그램이 실행되는 경로를 저장한다.
		perror("getcwd error");
	}

        for(i=1; i<argc; i++){ //argc의 수만큼 반복문을 실행합니다.
		if(!strcmp(argv[i], "-o")){ //옵션 -o일 경우
			Ooption = 1; // -o 옵션 설정 값을 1로 설정
                        start_p = i+1; // 합쳐야 될 첫 번째 파일의 인수 위치를 설정합니다.

                        while(argv[i+1][j]!='1' && argv[i+1][j]!='\0'){
	//절단된 파일의 첫파일은 마지막에 1이 들어가므로 '\0'과 '1'을 마나게 되면 while문을 빠져나가게 됩니다.
				j++; // i+1 argv[i+1]의 문자의 수를 카운팅합니다.
			} 
			TempFileName = (char *)malloc(j-1);
                        // 카운팅된 수-1의 크기만큼 동적할당을 합니다. (default 생성파일 이름으로 설정)
			strncpy(TempFileName, argv[i+1], j-2); //argv[i+1]의 문자열을 TempFileName에 복사해줍니다. j-2만큼 복사를 해줌으로 뒤의 숫자는 빼주고 복사를 해주게 됩니다.


			final_p = i;
                        while(strncmp(argv[final_p+1],"-", 1)){
			//다음 인자의 첫 문자와 "-" 를 비교해 참이면 실행(합쳐야될 파일들을 계속 받기 위함)
                        	final_p++; // 합쳐야 될 마지막 파일의 인수 위치 저장
                                if((final_p+1)>= argc) 
				// 인자의 값이 입력받은 인자의 갯수보다 많거나 같으면 while문을 탈출.
                                 	break;
                        }
                }
                if(!strcmp(argv[i], "-n")){//-n의 문자일 경우
                        Noption = 1;             //-n 옵션 설정 값을 1로 설정
                        SetFileName = argv[i+1]; //입력받은 i+1의 위치의 문자열을 파일의 이름으로 설정합니다.
                }
                if(!strcmp(argv[i], "-d")){//-d의 문자일 경우
			Doption = 1;
                        dir = argv[i+1]; //입력받은 디렉토리
			
                      
                }
                if(!strcmp(argv[i], "-x")){//-x의 문자일 경우
                        Xoption = 1; // -x 옵션 설정 값을 1로 설정
                }
        }


        if(Noption == 0){ // -n 옵션이 없을 경우 Default Name을 생성될 생성될 파일의 이름으로 설정해줍니다.
		printf("이름 지정 옵션(-n) : X\n");
                strcpy(SetFileName, TempFileName);
        }
	if(Noption == 1){ // -n 옵션이 있을 경우 
                printf("이름 지정 옵션(-n) : O\n");
        }

        if(Ooption == 0){ // -o 옵션은 필수 옵션이기 때문에 프로세스 실행 불가
                printf("-o 옵션을 사용하셔야합니다. -o 옵션을 사용하세요.\n");
                exit(1);
        }
                printf("합칠 파일 옵션(-o) : O\n");//-o 옵션이 있을 경우 출력
	if(Doption == 0)
                printf("디렉토리 옵션(-d) : X\n");//-d 옵션이 없을 경우 출력
        if(Doption == 1){
                printf("디렉토리 옵션(-d) : O\n");//-d 옵션이 있을 경우 출력
		if(chdir(dir) == -1){ // -d 옵션 이후 입력 받은 디렉토리 이름이 없을 경우
                          mkdir(dir, 0777); // 입력 받은 디렉토리명으로 디렉토리 생성
		}
	}
	if(Xoption == 0)
                printf("삭제 옵션(-x) : X\n");//-x 옵션이 없을 경우 출력
        if(Xoption == 1)
                printf("삭제 옵션(-x) : O\n");//-x 옵션이 있을 경우 출력

	buffer = (char *)malloc(sizeof(char)*SIZE); // 동적할당을 해줍니다. (기본 512)

	for(i = start_p; i <= final_p; i++){ // 합쳐야 할 파일의 첫 인자 위치 값 부터 마지막 인자 위치 값 까지 실행
		
                if((infile = open(argv[i], O_RDONLY)) == -1){
                        fputs("InFile open Error!! \n", stderr); //합쳐야할 파일읽기를 실패할 경우 출력.
                        return (-1);
                }
		if(Doption==1){
				chdir(dir); // 수행 디렉토리 변경			
		}
		printf("합치는 파일 이름 : %s\n", argv[i]);//파일의 이름을 출력
			
		if((outfile = open(SetFileName, O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1){
			// 루트사용자는 읽기, 쓰기 가능. 일반사용자와 그룹은 읽기만 가능 
                        fputs("OutFile open Error!! \n", stderr); // 생성될 파일 열기 실패
                        close(infile); // 생성될 파일을 열기 실패 하면 이미 열었던 합쳐야될 파일을 닫아준다.
                        return (-2);
                }
		if(Doption==1){
				chdir(PresentDir); // 수행 다시 이전의 디렉토리 변경해준다.			
		}
		if(Xoption == 1) // -x 옵션이 있을 경우
                        remove(argv[i]); // 기존에 있던 파일들을 지운다.
		while((nread = read(infile, buffer, SIZE)) > 0){ 
			if((out_t= write(outfile, buffer, nread)) < nread){
                                close (infile);//파일을 닫아준다.
                                close (outfile);//파일을 닫아준다.
                                free(buffer); //버퍼 동적해제
                                return (-3);
                        }
                }
                close (infile);//파일을 닫아준다.
                close (outfile);//파일을 닫아준다.
		
                if(nread == -1)
                        return (-4); // 마지막 읽기에서 오류 발생
        }
	
        free(buffer); //버퍼 동적해제
	free(TempFileName);//동적해제
	printf("실행완료\n");

        return 0;
}
