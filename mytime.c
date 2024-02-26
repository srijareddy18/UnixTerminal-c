#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>

void calculateUsage(char* command[]) {
	int status_of_process, child_process;
	long wall_clock_seconds, wall_clock_nanoseconds;
	double elapsed;
	struct rusage sparentCPUUsagen, childCPUUsage;

	child_process = fork(); 
	struct timespec start, stop;
	clock_gettime(CLOCK_REALTIME, &start);
	printf("%s\n", *command);
	if (child_process == 0) {
		if (execvp(command[0], &command[0]) == -1) {
			printf("Command couldn\'t be loaded.\n");
      return;
		}
	}
	else if(child_process == -1) {
		printf("Child process creation failed (Fork failed).\n");
	}
	else {
		wait(&status_of_process);
		if (!WIFEXITED(status_of_process)) {
			printf("Command execution exited with a failure.\n");
		}
		else{
			clock_gettime(CLOCK_REALTIME, &stop);
			// Calculate child process cpu usage.
			getrusage (RUSAGE_CHILDREN, &childCPUUsage);
			wall_clock_seconds = stop.tv_sec - start.tv_sec;
			wall_clock_nanoseconds = stop.tv_nsec - start.tv_nsec;
			elapsed = wall_clock_seconds + wall_clock_nanoseconds*1e-9;
			printf("User CPU Time       : %ld.%06ldu sec\n",childCPUUsage.ru_utime.tv_sec, childCPUUsage.ru_utime.tv_usec);
			printf("System CPU Time     : %ld.%06lds sec\n",childCPUUsage.ru_stime.tv_sec, childCPUUsage.ru_stime.tv_usec);
			printf("Wall clock time     : 0h:00m:0%5fs\n", elapsed);
		}
	}
	return;
}

int main(int argc, char* argv[])
{
	if (argv[1] == NULL) {
		printf("Please input a command to check the resource utilisation.\n");
		exit(0);
	}

	calculateUsage(&argv[1]);
	return 0;
}
