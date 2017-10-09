#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>

void sigint_handler(int sig);
volatile sig_atomic_t usr1Happened = 0;
sem_t* semaphore;
int numThreads = 0;
int i = 0;
pthread_t* tid;

void thread(void* arg) {

	printf("TID = %d: Thread created.\n", pthread_self());

	while(1) {
		//get access to the semaphore
		sem_wait(semaphore);

		printf("TID = %d: Has unblocked.\n", pthread_self());

		sleep(5);
	}
}

int main(void)
{
	struct sigaction sa;

	printf("Enter the number of threads to create:");
	scanf("%d", &numThreads);

	pthread_t array[numThreads];
	tid = array;

	//Initiate a single-lock semaphore
	//sem_init(&semaphore, 0, 1);

	semaphore = sem_open("/Lab3", O_CREAT, 0);

	sa.sa_handler = sigint_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("PID = %d: Parent spooling up...\n", getpid());

	for(i = 0; i < numThreads; ++i){
		if(pthread_create(&tid[i], NULL, &thread, NULL) != EOK) {
			perror("pthread_create");
			exit(1);
		}
	}

	for(i = 0; i < numThreads; ++i) {
		if(pthread_join(tid[i], NULL) != EOK) {
			perror("pthread_join");
			exit(1);
		}
	}

	/*while(!usr1Happened) {
		sleep(1);
	}*/

	return 0;
}


void sigint_handler(int sig) {
	if(sig == SIGUSR1){

		printf("PID = %d: SIGUSR1 received. Parent exiting.\n", getpid());
		fflush(stdout);

		for(i = 0; i < numThreads; ++i) {
			if(pthread_kill(tid[i], 2) != EOK) {
				perror("pthread_kill");
				exit(1);
			}
		}
	}


}
