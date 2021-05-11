#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define MAX_BUF 1024 
#define READ 0
#define WRITE 1

int main(){
        clock_t start, end;
        float res;
        int fdA[2],fdB[2]; // 프로세스간 소통을 위해 2개의 파이프 생성
        pid_t pid; // 프로세스 id
        char buf[MAX_BUF];
        int count=0;

        start = clock();

        // 예외처리
        if(pipe(fdA) < 0){ 
            printf("pipe error\n");
            exit(1);
        }

        if(pipe(fdB) < 0){
            printf("pipe error\n");
            exit(1);
        }

        if((pid=fork())<0){
            printf("fork error\n");
            exit(1);
        }

        printf("\n");
        if(pid>0){ // pid가 0보다 크면 부모 프로세스
            close(fdA[READ]);
            close(fdB[WRITE]);
            for (int i =0; i<500; i++){
                    sprintf(buf,"parent %d",count++); 
                    write(fdA[WRITE],buf,MAX_BUF); // 부모 프로세스에서 파이프에 데이터 작성.
                    memset(buf,0,sizeof(buf)); // buf 초기화
                    read(fdB[READ],buf,MAX_BUF); // 자식 프로세스에서 작성한 데이터를 buf에 저장
                    printf("parent got message : %s\n",buf); // 출력
            }
        }else{  // pid가 0이면 자식 프로세스
            close(fdA[WRITE]);
            close(fdB[READ]);
            count = 10000;
            for (int i =0; i<500; i++){
                    sprintf(buf,"child %d",count++);
                    write(fdB[WRITE],buf,MAX_BUF); // 자식 프로세스에서 파이프에 데이터 작성
                    memset(buf,0,sizeof(buf));  // 초기화
                    read(fdA[READ],buf,MAX_BUF); // 부모 프로세스에서 작성한 데이터를 buf에 저장
                    printf("\tchild got message : %s\n",buf); // 출력
            }
        }
    end = clock();
    res = (float)(end - start)/CLOCKS_PER_SEC;
    printf("%f\n", res);
}