#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


/*struct arguments {
int *sum;
int value;
};

void *do_work(void *arg);
int main(int argc, char *argv) {
pthread_t worker_thread1;
pthread_t worker_thread2;
struct arguments *arg[2];
int sum=0;
// Build argument to threads
arg[0] = (struct arguments *)calloc(1, sizeof(struct arguments));
arg[0]->value=3;
arg[0]->sum=&sum;
arg[1] = (struct arguments *)calloc(1, sizeof(struct arguments));
arg[1]->value=5;
arg[1]->sum=&sum;
// Create thread1
if (pthread_create(&worker_thread1, NULL,
do_work, (void *)arg[0])) {
fprintf(stderr,"Error while creating thread\n");
exit(1);
}
// Create thread2
if (pthread_create(&worker_thread2, NULL,
do_work, (void *)arg[1])) {
fprintf(stderr,"Error while creating thread\n");
exit(1);
}
// Join with thread
if (pthread_join(worker_thread1, NULL)) {
fprintf(stderr,"Error while joining with child thread\n");
exit(1);
}
if (pthread_join(worker_thread2, NULL)) {
fprintf(stderr,"Error while joining with child thread\n");
exit(1);
}
printf("\nSum: %d",sum);
exit(0);
}
void *do_work(void *arg) {
struct arguments *argument;
argument=(struct arguments*)arg;
int val=argument->value;
int *sum=argument->sum;
*sum+=val;
return NULL;
}*/
struct arguments {
pthread_mutex_t *mutex;
int val;
int *count;
int threadNum;
};
void *do_work(void *arg);
int main(int argc, char *argv) {
pthread_t worker_thread1;
pthread_t worker_thread2;
pthread_mutex_t lock1;
pthread_mutex_t lock2;
pthread_mutex_init(&lock1, NULL);
pthread_mutex_init(&lock2, NULL);
struct arguments *arg[2];
int count=0;
// Build argument to threads
arg[0] = (struct arguments *)calloc(1, sizeof(struct arguments));
arg[0]->mutex=&lock1;
arg[0]->val=2;
arg[0]->count=&count;
arg[0]->threadNum = 1;
arg[1] = (struct arguments *)calloc(1, sizeof(struct arguments));
arg[1]->mutex=&lock2;
arg[1]->val=3;
arg[1]->count=&count;
arg[1]->threadNum = 2;

// Create thread2
if (pthread_create(&worker_thread2, NULL,
do_work, (void *)arg[1])) {
fprintf(stderr,"Error while creating thread\n");
exit(1);
}

// Create thread1
if (pthread_create(&worker_thread1, NULL,
do_work, (void *)arg[0])) {
fprintf(stderr,"Error while creating thread\n");
exit(1);
}


// Join with thread
if (pthread_join(worker_thread1, NULL)) {
fprintf(stderr,"Error while joining with child thread\n");
exit(1);
}
if (pthread_join(worker_thread2, NULL)) {
fprintf(stderr,"Error while joining with child thread\n");
exit(1);
}
exit(0);
}
void *do_work(void *arg) {
struct arguments *argument;
argument=(struct arguments*)arg;
printf("\nThread %d entered do_work()\n", argument->threadNum);
pthread_mutex_t *mutex=argument->mutex;
int val=argument->val;
int *count=argument->count;
pthread_mutex_lock(mutex);
*count+=val;
printf("\nThread number: %d\n Count: %d",argument->threadNum,*count);
pthread_mutex_unlock(mutex);
return NULL;
}

