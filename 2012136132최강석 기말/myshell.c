#include "myshell.h"

int userin(char *p);
int gettok(char **outptr);
int inarg(char c);
int procline(void);
int runcommand(char **cline, int where);
int pipes(int num, int where);

int fatal(char *s)
{//오류처리
   perror(s);
   exit(1);
}

/* 프로그램 버퍼 및 작업용 포인터들  */
static char inpbuf[MAXBUF], tokbuf[2*MAXBUF], *ptr = inpbuf, *tok = tokbuf;

/* 프롬프트를 프린트하고 한 줄을 읽는다. */
int userin(char *p)
{
   int c, count;


   ptr = inpbuf;//뒤의 루틴들을 위한 입력 버퍼들을 초기화
   tok = tokbuf;//뒤의 루틴들을 위한 토큰 버퍼들을 초기화

   /* 프롬프트를 표시한다. */
   printf("%s",p);
   count = 0;

   while(1)
   {
      if ((c = getchar()) == EOF)//EOF를 만날 경우
         return (EOF);

      if (count < MAXBUF)/*카운트가 MAXBUF미만일 경우 count값을 ++해준다. */
         inpbuf[count++] = c;

      if(c == '\n' && count < MAXBUF)
      {/*\n을 만나지 않고 카운트가 MAXBUF미만일 경우*/
         inpbuf[count] = '\0';
         return count;
      }

      //줄이 너무 길면 재시작을 해준다
      /*라인에서 \n을 만날경우*/
      if (c == '\n')
      {// 
         printf("myshell: input line too long!\n");
         count = 0;
         printf("%s",p);
      }
   }
}

/* 토큰을 가져와서 tokbuf에 넣는다. */
int gettok (char **outptr)
{
   int type;
   int c;

   /* outptr 문자열을 tok로 지정한다. */
   *outptr = tok;

   /* 토큰을 포함하고 있는 버퍼로부터 여백을 제거한다. */
   while(*ptr == ' ' || *ptr == '\t')
      ptr++;

   /* 토큰 포인터를 버퍼내의 첫 토큰으로 지정한다. */
   *tok++ = *ptr;

   /* 버퍼내의 토큰에 따라 유형 변수를 지정한다.(pipe 추가) */
   switch(*ptr++){

   case '"':
   case '\n':
      type = EOL;
      break;

   case '&':
      type = AMPERSAND;
      break;

   case ';':
      type = SEMICOLON;
      break;

   case '\'':
      c = *(ptr-1);
      tok--;

      for(;;)
      {
         *tok++ = *ptr++;
         if(*ptr != c)
            break;
      }
      *ptr++;
      type = ARG;
      break;
   case '|':
      type = PIPE;
      break;
   case '#':
      type = PONDKEY;
      break;

   default:
      type = ARG;
      /* 유효한 보통문자들을 계속 읽는다. */
      while (inarg (*ptr))
         *tok++ = *ptr++;

   }

   *tok++ = '\0';
   return type;
}

/* 한문자가 보통인수의 일부가 될 수 있는지 결정. */
static char special[] = {' ','\t','&',';','\n','\0'};

int inarg(char c){
   char *wrk;

   for(wrk = special; *wrk; wrk++)
   {
      if(c == *wrk)
         return (0);
   }
   return (1);
}

char *arg1[10][MAXARG + 1];

/* 입력줄을 처리한다. */
int procline(void)
{
   char checkexit[] = "exit";
   char *arg2[MAXARG + 1];      // runcommand를 위한 포인터 배열 
   int toktype;             // 명령내의 토큰의 유형 
   int narg;                // 지금까지의 인수 수 
   int type1;                // FOREGROUND 또는 BACKGROUND 
   int type2;
   int num = 0;                // 파이프 개수
   int i, j;

   for (i=0; i<10; i++)
   {
      for(j=0; j<MAXARG+1; j++)
         arg1[i][j]=0;
   }
   type2=0;
   narg = 0;

   for(;;)
   {
      /* 토큰 유형에 따라 행동을 취한다 */
      switch(toktype = gettok(&arg2[narg]))
      {
      case ARG://argument일 경우
         if(narg < MAXARG)
            narg++;
         break;
      case EOL:
      case SEMICOLON:

      case AMPERSAND://&의 경우(백그라운드)
         if(toktype == AMPERSAND)
            type1 = BACKGROUND;//타입을 백그라운드로 설정
         else
            type1 = FOREGROUND;//다입을 포그라운드로 설정

         if(narg != 0)
         {
            int m = 1;
            int fd1, fd2;//파일 디스크립터
            pid_t pid;
            char **temp;
            arg2[narg] = NULL;
            temp = arg2;
            while ((m <= narg) && ((strcmp (temp[m-1], "<") * strcmp (temp[m-1], ">")) != 0))
               m++;

            if (strcmp(arg2[0], "cd") == 0)//cd의 함수에 대해서 설정.
            {
               if (narg != 1)//narg가 1이 아닐 경우에
               {
                  if (chdir(arg2[1]) == -1)
                     printf("Not exist\n");
               }else{
                  chdir(getenv("HOME"));//환경변수의 값을 읽어온다.
               }
            }
            /* 파이프 명령의 경우 */
            else if(type2 == PIPE)
            {
               for(i=0; i<narg; i++)
                  arg1[num][i] = arg2[i];
               pipes(num, type1);//type1에 값에 대해 pipes}
            }else{//파이프의 명령이 아닐 경우
               if (strcmp (checkexit, *arg2 ) == 0)
                  exit(1);
               runcommand(arg2, type1);// type1에 값에 대해 runcommand
            }
         }
         
         if((toktype == EOL) || (toktype == PONDKEY))
         return 0;
         
         narg = 0;
         break;

      case PIPE:
         for (i = 0; i < narg; i++)
            arg1[num][i] = arg2[i]; // 파이프 문자열을 저장   
         arg1[num][narg] = NULL;
         num++;
         narg = 0;
         type2 = toktype;//토큰타입을 설정한다.
         break;
      case PONDKEY:
         break;
      }
   }
}

/* wait를 선택사항으로 하여 명령을 수행한다. */
int runcommand(char **cline, int where)
{
   pid_t pid;
   int status;//피드의 상태

   if(!strcmp(*cline,"logout")) //logout의 명령을 받을 경우에
      exit(1);//종료한다.
   switch (pid = fork()) //피드 생성
   {
   case -1:
      perror ("smallsh");
      return (-1);
      /* 부모의 코드를 의미한다. */
   case 0:
      if(where == BACKGROUND){ //백그라운드
         signal(SIGINT, SIG_IGN);;//SIGINT가 먹히지 않게 설정
         signal(SIGQUIT, SIG_IGN);;//SIGQUIT가 먹히지 않게 설정
      }else{//포그라운드
         signal(SIGINT, SIG_DFL);//SIGINT가 먹히게 설정
         signal(SIGQUIT,SIG_DFL);//SIGQUIT가 먹히게 설정
      }
      execvp(*cline,cline);//명령 라인에서 받아 들인 인수에 해당되는 프로그램을 실행
      perror(*cline);
      exit(1);
   default://그 이외의 경우 signal이 먹히지 않게 설정
      signal(SIGINT, SIG_IGN);
      signal(SIGQUIT, SIG_IGN);
   }
   /* 만일 백그라운드 프로세스이면 프로세스 식별자를 프린트하고 퇴장한다. */
   if(where == BACKGROUND)
   {
      printf ("[Process id %d]\n", pid);
      return (0);
   }

   /* 프로세스 pid가 퇴장할 때까지 기다린다. */
   if (waitpid(pid,&status,0) == -1){
      return (-1);
   }
   else
      return (status);
}
//파이프 함수.
int pipes(int num, int where)
{
   pid_t pid[num+1];
   int i=0;
   int j;
   int p[num][2];
   int status;

   /* n개의 파이프 생성. */
   for(i = 0; i < num; i++)
   {
      if(pipe(p[i])==-1)
         fatal("pipe call in join");
   }

   for(i = 0; i <= num; i++)//num만큼 반복
   {
      switch (pid[i] = fork()){//fork() 발생
      case -1://fork 생성에러
         fatal("Children failed");
      case 0://0이면 자식 프로세스
         if(where == BACKGROUND)
         {/*백그라운드의 경우*/
            signal (SIGINT,SIG_IGN);//SIGINT가 먹히지 않게 설정
            signal (SIGQUIT,SIG_IGN);//SIGQUIT가 먹히지 않게 설정
         }
         else
         {   /*포그라운드의 경우*/
            signal (SIGINT, SIG_DFL);//SIGINT가 먹히게 설정
            signal (SIGQUIT, SIG_DFL);////SIGQUIT가 먹히게 설정
         }
         if(i==0)
         {/*출력이 p[i][1]로 향하게 된다. */
            dup2(p[i][1],1);//stdout
            for(j=0; j < num; j++)
            {
               close(p[j][1]);
               close(p[j][0]);
            }
            execvp(arg1[i][0], arg1[i]);//명령 라인에서 받아 들인 인수에 해당되는 프로그램을 실행
            fatal(arg1[i][0]);
         }//마지막 num일 경우
         else if (i == num)
         {
            dup2(p[i-1][0],0);//stdin
            for(j = 0;j < num; j++)
            {   
               close(p[j][0]);
               close(p[j][1]);
            }
            execvp(arg1[num][0], arg1[num]);//명령 라인에서 받아 들인 인수에 해당되는 프로그램을 실행
            fatal(arg1[num][0]);
         }
         else//0과 num이 아닐 경우.
         {
            dup2(p[i-1][0],0);//stdin
            dup2(p[i][1],1);//sdout
            for(j = 0; j < num; j++)
            {
               close(p[j][0]);
               close(p[j][1]);
            }
            execvp(arg1[i][0], arg1[i]);//명령 라인에서 받아 들인 인수에 해당되는 프로그램을 실행
            fatal(arg1[i][0]);
         }
      }
   }
   /* 부모 프로세스의 경우 */
   for(j = 0; j < num; j++)
   {
      close(p[j][0]);
      close(p[j][1]);
   }
   /* BACKGROUND의 경우 자식 프로세스 아이디 출력. */
   if (where == BACKGROUND)
   {
      for(j = 0; j <= num; j++)
      {
         if (pid[j]>0)
            printf("[Process id %d]\n", pid[j]);//프로세스 아이디를 출력
         else
            sleep(1);
      }
      return(0);
   }
   while(waitpid(pid[num], &status, WNOHANG) == 0)
      //프로세스의 종료를 / 기다립니다.
      sleep(1);
   return(0);
}

char *prompt = " Prompt> "; // 프롬프트

void main()
{

   signal(SIGINT, SIG_IGN);//SIGINT가 먹히지 않게 설정
   signal(SIGQUIT, SIG_IGN);//SIGOUT가 먹히지 않게 설정

   while (userin(prompt) != EOF)
   {
      procline();//procline실행
   }
}