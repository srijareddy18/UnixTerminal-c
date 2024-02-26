#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <fcntl.h>

char *command_arguments[1024];
int count_command_arguments = 0;
pid_t childprocess_mtimeout;
bool double_redirection_check = false, io_redirection_check = false, pipe_check = false;
char* redirection = ' ';
int input_redirection_location = 0, output_redirection_location = 0, double_redirection_location = 0, pipe_commands_count = 0;
int pipe_positions[1024] = {0};

void split_command(char* command[80]) {
  int i = 0;
  command_arguments[i] = strtok(command, " ");
  do{
    count_command_arguments++;
  } while((command_arguments[count_command_arguments] = strtok(NULL, " ")) != NULL);

  return;
}

bool check_for_io() {
  int i=0,j=0;
  while(i < count_command_arguments){
    if(strcmp(command_arguments[i], "<") == 0) {
      redirection = "<";
      input_redirection_location = i;
      for (j=i; j< count_command_arguments; j++)
      {
        if(strcmp(command_arguments[j], ">") == 0)
        {
          double_redirection_check = true;
          double_redirection_location = j;
          return true;
        }
      }
      return true;
    }
    else if(strcmp(command_arguments[i], ">") == 0) {
      output_redirection_location = i;
      redirection = ">";
      for (j=i; j< count_command_arguments; j++)
      {
        if(strcmp(command_arguments[j], "<") == 0)
        {
          double_redirection_check = true;
          double_redirection_location = j;
          return true;
        }
      }
      return true;
    }
    else if(strcmp(command_arguments[i], "|") == 0) {
      pipe_check = true;
      pipe_positions[pipe_commands_count] = i;
      pipe_commands_count++;
    }
    i++;
  }
  return false;
}

void pipe_execution() {
  int fd[pipe_commands_count][2];
  int i = 0, status_of_process, j=0,k=0,l=0;
  int output_stream = dup(STDOUT_FILENO);
  int input_stream = dup(STDIN_FILENO);
  char* args[1024];
  for( i = 0; i < pipe_commands_count ; i++ ){
    if( pipe(fd[i]) == -1 ){
      printf("Pipe creation failed..\n");
      return;
    }
  }
  i=0;
  while( i <= pipe_commands_count) {
    pid_t child = fork();
    l=0;
    if(child == 0) {
      if(i > 0) {
        dup2(fd[i-1][0], STDIN_FILENO);
        close(fd[i-1][0]);
      }
      if(i < pipe_commands_count) {
        dup2(fd[i][1], STDOUT_FILENO);
        close(fd[i][1]);
      } 
      if(i == 0) {
        j = pipe_positions[i];
        k = 0;
      }
      else if(i == pipe_commands_count) {
        j = count_command_arguments;
        k = pipe_positions[i-1]+1;
      }
      else {
        j = pipe_positions[i];
        k = pipe_positions[i-1]+1;
      }
      while(k < j) {
        args[l++]= command_arguments[k++];
      }
      args[l]=NULL;
      execvp(args[0], &args);
    }
    else if(child == -1 ) {
      printf("Fork failed..\n");
    }
    else {
      if(i < pipe_commands_count) {
        close(fd[i][1]);
      }
      if(waitpid(child, NULL, 0) == -1)
      {
        printf("Process execution failed..\n");
      }
    }
    i++;
  }

  for (i = 0; i < pipe_commands_count; i++) {
		close(fd[i][0]);
		close(fd[i][1]);
	}
	return 0;
}

void double_redirection() {
  int input_file_loc, input_file, status_of_process, i, read_io=0;
  int output_file_loc, output_file;
  int input_stream = dup(STDIN_FILENO);
  int output_stream = dup(STDOUT_FILENO);
  pid_t corp=fork();
  char* args[1024];
  if(strcmp(redirection, ">")==0)
  {
    input_file_loc = double_redirection_location + 1;
    output_file_loc = double_redirection_location - 1;
  }
  else if(strcmp(redirection, "<") == 0)
  {
    input_file_loc = double_redirection_location - 1;
    output_file_loc = double_redirection_location + 1;
  }

  input_file = open(command_arguments[input_file_loc], O_RDONLY);
  output_file = open(command_arguments[output_file_loc], O_WRONLY | O_CREAT, 0744);
  if (corp == 0){
    dup2(input_file, STDIN_FILENO);
    dup2(output_file, STDOUT_FILENO);
    close(input_file);
    close(output_file);

    for (i=0; i < (double_redirection_location - 2); i++){
      args[i]= command_arguments[i];
    }

    execvp(args[0], args);     
  }
  else{
    close(input_file);
    close(output_file);
  }
  wait(&status_of_process);
  if (!WIFEXITED(status_of_process)) {
    printf("Input/Output redirection execution exited with a failure.\n");
  }
  else {
    printf("Executed successfully input/output redirection..\n");
  }
  dup2(STDOUT_FILENO, output_stream);
  dup2(STDIN_FILENO, input_stream);
}

void input_redirection() {
  int file=open(command_arguments[input_redirection_location+1], O_RDONLY);
  pid_t corp=fork();
  int i=0, status_of_process;
  char* args[1024];
  int input_stream = dup(STDIN_FILENO);
  if (corp==0){
    close(input_stream);
    dup2(file, STDIN_FILENO);
    close(file);
    
    for (i=0; i<input_redirection_location; i++){
      args[i]=command_arguments[i];
    }
    execvp(command_arguments[0], args);        
  }
  else{
    close(file);
  }
  wait(&status_of_process);
  if (!WIFEXITED(status_of_process)) {
    printf("Input redirection execution exited with a failure.\n");
  }
  else {
    printf("Executed successfully input redirection..\n");
  }
  dup2(STDIN_FILENO, input_stream);
}

void output_redirection() {
  int file=open(command_arguments[output_redirection_location+1], O_WRONLY | O_CREAT, 0744);
  int i = 0, status_of_process;
  pid_t corp=fork();
  char* args[1024];
  int output_stream = dup(STDOUT_FILENO);
  if (corp==0){
    close(output_stream);
    dup2(file, STDOUT_FILENO);
    close(file);

    for (i=0; i < output_redirection_location; i++){
      args[i]=command_arguments[i];
    }
    execvp(command_arguments[0], args);
  }else{
    close(file);
  }
  wait(&status_of_process);
  if (!WIFEXITED(status_of_process)) {
    printf("Output redirection execution exited with a failure.\n");
  }
  else {
    printf("Executed successfully output redirection..\n");
  }
  dup2(STDOUT_FILENO, output_stream);
}

void mypwd() {
  const char *pwd = getenv("PWD");
  printf("%s\n",pwd);
  return;
}

void myexit() {
  exit(0);
  return;
}

void signalHandler(int signum){
	printf("Command %s killed before complete execution");
	kill(childprocess_mtimeout, SIGKILL);
}

void mycd() {
  char *dir;
  dir = command_arguments[1];
  const char* newdir[1024];
  int success_dir_change = 0;
  char *home = getenv("HOME");

  if(count_command_arguments < 2) {
    char *current_pwd = getenv("PWD");
    if(strcmp(home, current_pwd) == 0){
      return;
    }
    success_dir_change = chdir(home);
    if(success_dir_change == -1) {
      perror("Error in changing directory.\n");
      return;
    }
    else {
      getcwd(newdir, sizeof(newdir));
      setenv("PWD", newdir, 1);
      return;
    }
  }

  DIR *directory = opendir(dir);

  if(directory == NULL) {
    printf("Not a valid directory\n");
  }
  else {
    success_dir_change = chdir(dir);
  }

  //Check for successful change in the directory
  //If yes, update the pwd environmental variable to current path

  if(success_dir_change == -1) {
    perror("Error in changing directory.\n");
  }
  else {
    getcwd(newdir, sizeof(newdir));
    setenv("PWD", newdir , 1);
    printf("%s\n", getenv("PWD"));
  }
  return;
}

void my_prompt() {  
  count_command_arguments = 0;
  double_redirection_check = false;
  io_redirection_check = false;
  redirection = ' ';
  double_redirection_location = 0;
  output_redirection_location = 0;
  input_redirection_location = 0;
  pipe_commands_count = 0;
  pipe_check = false;
  printf("\n$");
  return;
}

void external_commands_execution() {
  pid_t exertnal_command;
  int status_of_process;
	exertnal_command = fork();
			
	if(exertnal_command == 0){	
		execvp(command_arguments[0], command_arguments);
	}else if(exertnal_command == -1){          
	    printf("Fork failed, now exiting\n"); 
	}else{
     wait(&status_of_process);
      if (!WIFEXITED(status_of_process)) {
        printf("External Command execution exited with a failure.\n");
      }
      else {
        printf("Successfully executed external command.\n");
      }
	}
}

int main()
{
  char *cmd[80];
  int i = 0;
  my_prompt();
  while(gets(&cmd) != NULL) {
    if(strlen(cmd) == 0)
    {
      my_prompt();
      continue;
    }
    
    split_command(cmd);
    io_redirection_check = check_for_io();
    if(io_redirection_check == true && double_redirection_check == true)
    {
      double_redirection();
    }
    else if(io_redirection_check == true && double_redirection_check == false)
    {
      if(strcmp(redirection, "<") == 0)
        input_redirection();
      else if(strcmp(redirection, ">") == 0)
        output_redirection();
    }
    else if(pipe_check == true) {
      pipe_execution();
      
    }

    else if(strcmp(command_arguments[0], "mycd") == 0){
      mycd();
    }
    else if(strcmp(command_arguments[0], "mypwd") == 0) {
      mypwd();
    }
    else if(strcmp(command_arguments[0], "myexit") == 0) {
      myexit();
    }
    else if(strcmp(command_arguments[0], "mytime")==0){
			pid_t childprocess_mtimes;
			childprocess_mtimes=fork();	
			if(childprocess_mtimes==0){
				execl("mytime.x" ,"mytime.x" , command_arguments[1]);
			}else if(childprocess_mtimes== -1){
			    printf("Fork failed, now exiting\n");
			} else{
				waitpid(childprocess_mtimes,NULL, 0);
			}
		}
    else if(strcmp(command_arguments[0], "mymtimes")==0){
			pid_t childprocess_mtimes;
			childprocess_mtimes=fork();	
			if(childprocess_mtimes==0){
				execl("mymtimes.x" ,"mymtimes.x" , command_arguments[1]);
			}else if(childprocess_mtimes== -1){
			    printf("Fork failed, now exiting\n");
			} else{
				waitpid(childprocess_mtimes,NULL, 0);
			}	
		}
    else if(strcmp(command_arguments[0], "mytree")==0){
			pid_t childprocess_mtimes;
			childprocess_mtimes=fork();	
			if(childprocess_mtimes==0){
				execl("mytree.x" ,"mytree.x" , command_arguments[0]);
			}else if(childprocess_mtimes== -1){
			    printf("Fork failed, now exiting\n");
			} else{
				waitpid(childprocess_mtimes,NULL, 0);
			}	
		}
    else if(strcmp(command_arguments[0], "mytimeout")==0){
      // mytimeout();	
      pid_t childprocess_mtimes;
			childprocess_mtimes=fork();	
			if(childprocess_mtimes==0){
				execvp("mytimeout.x" , &command_arguments[0]);
			}else if(childprocess_mtimes== -1){
			    printf("Fork failed, now exiting\n");
			} else{
				waitpid(childprocess_mtimes,NULL, 0);
			}
		}
    else{
      external_commands_execution();
    }
    my_prompt();
  }
  printf("EXITING...\n");
  return 0;
}
