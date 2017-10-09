#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

//Variable used to determine if a trigger has happened
sig_atomic_t usr1Happened = 0;

//handler
void sigint_handler(int sig);

int main(int argc, char *argv[]) {

	pid_t PID = getpid();
	struct sigaction sa;
	sigset_t set;

	sa.sa_handler = sigint_handler;
	sa.sa_flags = 0;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	//Check to make sure the handler is correctly registered
	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("PID=%u:Running...\n", PID);

	while(!usr1Happened) {
		sleep(1);
	}

	printf("PID=%u:Received USR1.\n", PID);
	printf("PID=%u:Exiting.\n", PID);

	return 0;
}

void sigint_handler(int sig) {
	usr1Happened = 1;
}
