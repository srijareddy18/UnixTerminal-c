#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int count_command_arguments=0;
pid_t childprocess_mtimeout;

void signalHandler(int signum){
	printf("Command %s killed before complete execution", childprocess_mtimeout);
	kill(childprocess_mtimeout, SIGKILL); //SIGTERM
}

void mytimeout(char* command_arguments[]) {
  int time_to_run = 0;
  int status_of_process, i = 0, position_minutes = 0, position_hours = 0, position_days = 0;
  char *minutes = "m";
  char *hours = "h";
  char *days = "d";
  char *position_of_minutes, *position_of_hours, *position_of_days;

  if(count_command_arguments > 2){
    time_to_run = atoi(command_arguments[0]);
    position_of_minutes = strchr(command_arguments[0], minutes[0]);
    position_of_hours = strchr(command_arguments[0], hours[0]);
    position_of_days = strchr(command_arguments[0], days[0]);
    position_minutes = (position_of_minutes == NULL ? -1 : position_of_minutes - command_arguments[1]);
    position_hours = (position_of_hours == NULL ? -1 : position_of_hours - command_arguments[1]);
    position_days = (position_of_days == NULL ? -1 : position_of_days - command_arguments[1]);
    if( position_minutes > 0) {
      time_to_run = time_to_run * 60;
    }
    else if(position_hours > 0) {
      time_to_run = time_to_run * 3600;
    }
    else if(position_days > 0) {
      time_to_run = time_to_run * 86400;
    }

    childprocess_mtimeout = fork();
    if(time_to_run == 0)
    {
      printf("Please input valid time interval.\n");
    }
    else if(childprocess_mtimeout == 0){
      signal(SIGALRM, signalHandler);
      alarm(time_to_run);
      execvp(command_arguments[1], &command_arguments[1]);
    }
    else if(childprocess_mtimeout == -1){
      printf("Fork failed, now exiting\n");
    }
    else{
      wait(&status_of_process);
      if (!WIFEXITED(status_of_process)) {
        printf("Command execution exited with a failure.\n");
      }
      else {
        printf("process executed within time.\n");
      }
    }
  }
  else {
    printf("Timeout requires minimum two arguments. Please input duration along with the command to execute.\n");
  }
}

int main(int argc, char* argv[]) {
    if (argv[1] == NULL) {
      printf("Timeout requires minimum two arguments.\n");
      exit(0);
	  }
    count_command_arguments = argc;
    mytimeout(&argv[1]);
    return 0;
}

