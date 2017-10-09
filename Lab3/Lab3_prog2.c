#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>

sem_t* semaphore;

int main(int argc, char *argv[]) {

	int wakeUp = 0;
	int index = 0;

	printf("PID = %d: Semaphore value changing program started.\n", getpid());

	semaphore = sem_open("/Lab3", O_CREAT, 0);

	do {
		wakeUp = 0;
		printf("Please enter the number of threads you would like to wake up (0 to exit): ");
		fflush(stdout);
		scanf("%d", &wakeUp);

		for(index = 0; index < wakeUp; ++index) {
			sem_post(semaphore);
		}

	} while(wakeUp);

	return(0);
}
