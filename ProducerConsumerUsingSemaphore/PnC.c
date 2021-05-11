#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#define TRUE 1
#define FALSE 0
#define N 32 // buffer size
#define ITEMS 1000 // 연산 횟수
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif
// mac에서 세마포어 함수를 사용하기 위한 설정
struct rk_sema {
#ifdef __APPLE__
    dispatch_semaphore_t    sem;
#else
    sem_t                   sem;
#endif
};


static inline void
rk_sema_init(struct rk_sema *s, uint32_t value)
{
#ifdef __APPLE__
    dispatch_semaphore_t *sem = &s->sem;

    *sem = dispatch_semaphore_create(value);
#else
    sem_init(&s->sem, 0, value);
#endif
}

static inline void
rk_sema_wait(struct rk_sema *s)
{

#ifdef __APPLE__
    dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
#else
    int r;

    do {
            r = sem_wait(&s->sem);
    } while (r == -1 && errno == EINTR);
#endif
}

static inline void
rk_sema_post(struct rk_sema *s)
{

#ifdef __APPLE__
    dispatch_semaphore_signal(s->sem);
#else
    sem_post(&s->sem);
#endif
}
struct rk_sema mutex; // mutex 변수 선언

#define down(A) rk_sema_wait(A) // sem_wait 
#define up(A) rk_sema_post(A) // sem_post
int produce_item();
void consume_item(int consumer_id, int item);
void insert_item(int item);
int remove_item();
void producer(void);
void* consumer(void *cid);
int buff[N];
int rear = -1;
int front = -1;
int main()
{
 	int i;
 	pthread_t t1, t2, t3, t4, t5; 
 	pthread_attr_t attr;
 	void *status;
 	int j;
 	// Initialize
    rk_sema_init(&mutex, 1);

    int thread1 = 1;
    int thread2 = 2;
    int thread3 = 3;
    int thread4 = 4;
    int thread5 = 5;
 	buff[0] = 0;
 	pthread_attr_init(&attr);
 	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); 
 	// Create a child thread for the consumer 
 	pthread_create(&t1, &attr, consumer, &thread1);//자식스레드 생성
 	pthread_create(&t2, &attr, consumer, &thread2);//자식스레드 생성
 	pthread_create(&t3, &attr, consumer, &thread3);//자식스레드 생성
 	pthread_create(&t4, &attr, consumer, &thread4);//자식스레드 생성
 	pthread_create(&t5, &attr, consumer, &thread5);//자식스레드 생성
    producer();


 	pthread_join(t1, &status);
 	pthread_join(t2, &status);
 	pthread_join(t3, &status);
 	pthread_join(t4, &status);
 	pthread_join(t5, &status);
    }

void producer(void)
{
 	static int nitem = 0;
 	while (nitem < ITEMS) {
	down(&mutex);  // 대기
 	insert_item(nitem); // 버퍼에 숫자 넣기
	up(&mutex); // 작업 끝
 	nitem++;
 	}
	down(&mutex);
 	insert_item(-1); // -1 means 'END'
	up(&mutex);
}
void* consumer(void *cid)
{
 	int consumer_id = *(int *) cid;
 	int item;
 	while (TRUE) {
		item = remove_item(); // 버퍼에서 숫자 빼오기
		if(item) {
			if (item == -1) // if 'END' mark 끝났다.
			exit(1);
			down(&mutex); // 대기
			consume_item(consumer_id, item); // 숫자 출력
			sleep(1);  
		 	up(&mutex); // 작업 끝
		}
 		
 	}
 	pthread_exit(NULL);
}

void consume_item(int consumer_id, int item)
{
 	printf("Consumer %d consumes: %d\n", consumer_id, item);
}
void insert_item(int item)
{
 	rear = (rear + 1) % N; // 순환 버퍼
 	buff[rear] = item;
 	return;
}
int remove_item()
{
 	int item;
 	front = (front + 1) % N;
 	item = buff[front];
 	return item;
}