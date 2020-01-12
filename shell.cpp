/*
COURSE: duke ECE551
NAME: Zihao Wu
NetID: zw154
 */
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "command.h"
////////////////////////////////testing options here/////////////////////////////////

//uncomment this will print all arguments taken by a command object without executing the program
//#define cmd_args
//uncomment this will only allows exit, update environment variables and check if the line translation works properly
//#define line_translation

///////////////////////process functions go here//////////////////////////////
/*
NAME
parent_process -- a parent process waiting execution of a child process
DESCRPTION
Provide child's pid "cpid", to let the waitpid() function to know which process the parent is waiting for   
 */
void parent_process(pid_t cpid) {
  pid_t w;      //wait pid
  int wstatus;  //wait status

  //printf("Parrent PID is %ld\n", (long)getpid());

  do {
    w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
    if (w == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
    }
    //wstatus
    if (WIFEXITED(wstatus)) {  //success or failure
      printf("Program exited with status %d\n", WEXITSTATUS(wstatus));
    }
    else if (WIFSIGNALED(wstatus)) {  //killed by signal such as kill -TERM
      printf("Program was killed by killed by signal %d\n", WTERMSIG(wstatus));
    }
    else if (WIFSTOPPED(wstatus)) {  //stopped by signal such as kill -STOP
      printf("Program was stopped by signal %d\n", WSTOPSIG(wstatus));
    }
    else if (WIFCONTINUED(wstatus)) {  //resume by signal such as kill -CONT
      printf("continued\n");
    }
  } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
  //allows for only a successful exit or termination signal
}

/*
NAME
child_process -- instantiate a command object, and execute the command
DESCRPTION
To instantiate a command object, provide information involving command line, PATH(program path), and exportedenvironment variables   
 */
void child_process(char * line,
                   const char * cmd_path,
                   const var_val & expt,
                   const std::string cdir) {
  //printf("Child PID is %ld\n", (long)getpid());
  command cmdLine(line, cmd_path, expt, cdir);

#ifdef cmd_args
  std::cout << "cmd info: " << std::endl;
  cmdLine.print_info();  //print infomation required by a command
  exit(EXIT_SUCCESS);
#else
  cmdLine.execute();  //execute command line
#endif
}

/*
NAME
pc_processes -- fork the current process, then run parent and children processes separately
DESCRIPTION
To run a command, the necessary information are command line string, program path (PATH), the
environment variables which are exported  
 */
void pc_processes(char * line,
                  const char * cmd_path,
                  const var_val & expt,
                  const std::string cdir) {
  pid_t cpid = fork();  //forked children pid

  if (cpid == -1) {  //return -1 for fork failure
    perror("fork");
    free(line);  //not sure here !!!!
    exit(EXIT_FAILURE);
  }
  else if (cpid == 0) {                         //return 0 in the child process
    child_process(line, cmd_path, expt, cdir);  // return success or failure automatically
  }
  else {  //return child's pid for parent process
    parent_process(cpid);
  }
}
//////////////////////////directory furnctions go here/////////////////////////
/*
NAME
get_cdir -- to change directory and get current directory name
DESCRIPTION
Give "home" directory name which tells to change the home directory characters with ~
 */
std::string get_cdir() {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("no such directory");
  }

  std::string ans(cwd);
  return ans;
}
/*
NAME
update_cdir -- to update directory "cdir" with a with a  cd command line "input" 
DESCRIPTION
if "input" doesn't specify the directory, it would enter home directory
 */
void update_cdir(const arguments & input, std::string & cdir, const std::string & home) {
  int temp;
  if (input.size() == 1) {
    temp = chdir(home.c_str());  //if cd withouth argument, channge directory to home
  }
  else {  //cd with argument
    temp = chdir(input[1]);
  }
  if (temp == 0) {  //if chdir on success, update cdir name
    cdir = get_cdir();
  }
  else {
    perror("fail to change directory");
  }
}

void print_prompt(const std::string & cdir, const std::string & home) {
  size_t temp;
  std::string pcdir(cdir);
  if ((temp = pcdir.find(home)) != std::string::npos) {
    pcdir = pcdir.substr(temp + home.size());
    pcdir = "~" + pcdir;
  }
  std::cout << "myshell:" << pcdir << "$";
}
///////////////////////initialization///////////////////////////////////////
/*
NAME
initial 
DESCRIPTION
set up home path, current directory, and environment variables' container
 */
void initial(envArgs & envs, std::string & cdir, std::string & home) {
  struct passwd * pw = getpwuid(getuid());
  home = pw->pw_dir;                   //get home path e.g. /home/zw154
  cdir = get_cdir();                   //get current directory's path name
  char * ECE551PATH = getenv("PATH");  //assign PATH to  EXE551PATH
  std::string cmd_paths("set ECE551PATH " + std::string(ECE551PATH));
  arguments temp(cmd_paths.c_str(), ' ', '\0');
  envs.update_envs(temp, 0);  //add ECE551 to environment variable
}

///////////////////////main function goes here///////////////////////////////
/*
main function 
DESCRIPTION
when we input a command after prompt, envArgs object(environment variable container) will translate 
the command line by replacing $VAR with value. Then the translated command line will be used to 
instantiate an arguments object. By looking into the size of argument and argv[0], and we classify 
commands into "exit", "change directory", "do nothing", "manipulate environment variables" and "execute a program".
The shell is terminated by exit command/EOF(ctrl + d in stdin)
 */
int main(void) {
  //set up informatin and ECE551PATH
  std::string home;
  std::string cdir;  //current directory

  envArgs envs;  //the object will store arguments set and arguments export separately
  initial(envs, cdir, home);
  print_prompt(cdir, home);

  //the body part of the shell
  char * line = NULL;
  size_t sz;
  while (getline(&line, &sz, stdin) != -1) {  //1. exit if input EOF

    //for command line argument test
#ifdef line_translation
    std::cout << "command line: " << line << std::endl;
#endif

    envs.translate_line(line);
    //translation: replace $VAR with value for a command line
#ifdef line_translation
    std::cout << "after translation: " << line << std::endl;
#endif
    arguments input(line, ' ', '\n');  //split command line in arguments
    int op;                            //to tell how to update envs, set/export/inc/env
    if (input.size() == 0) {           //2. do nothing if input \n or empty $VAR
      //do nothing
    }
    else if (strcmp(input[0], "exit") == 0) {  //3. exit if input exit
      free(line);
      return EXIT_SUCCESS;
    }
    else if (strcmp(input[0], "cd") == 0) {  //4. change directory
      update_cdir(input, cdir, home);        //update current directory
    }

    else if ((op = input.check_envCmd()) != -1) {  //5. manipulate enviroment variables
      envs.update_envs(input, op);
    }
#ifndef line_translation
    else {  //6. execute a program
      std::string cmd_paths(
          "$ECE551PATH");      //the look up function requires input that starts with $
      envs.lookup(cmd_paths);  //check ECE551PATH every time before execution

      pc_processes(
          line, cmd_paths.c_str(), envs.pass_expt(), cdir);  //execute parent and child processes
    }
#endif
    print_prompt(cdir, home);
  }
  free(line);

  return EXIT_SUCCESS;
}
