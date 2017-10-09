#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


void sigint_handler(int sig);
volatile sig_atomic_t usr1Happened = 0;

int main(void)
{
	int i = 0;
	int numChildren = 0;

	printf("Enter the number of children:");
	scanf("%d", &numChildren);

	struct sigaction sa;

	sa.sa_handler = sigint_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	printf("PID = %d: Parent spooling up...\n", getpid());

	for(i = 0; i < numChildren; ++i){

		switch(fork()) {
		case -1:
			perror("fork");  /* something went wrong */
			exit(1);		 /* parent exits */

		case 0:
			if (sigaction(SIGUSR1, &sa, NULL) == -1) {
				perror("sigaction");
				exit(1);
			}
			printf("PID = %d: Child running...\n", getpid());

			while (!usr1Happened){
				sleep(1);
			}

			printf("PID = %d: Child received USR1.\n", getpid());
			printf("PID = %d: Child exiting.\n", getpid());
			exit(0);

		default:
			break;

		}
	}

	for(i = 0; i < numChildren; ++i){
		wait(NULL);
	}

	printf("PID = %d: Children Killed... Parent exiting.\n", getpid());

	return 0;
}


void sigint_handler(int sig) {
	if(sig == SIGUSR1)
		usr1Happened = 1;

}
