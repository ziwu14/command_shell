#include "command.h"

#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>

#include <string>
#include <utility>
/*
NAME
execute -- execute the command
DESCRIPTION
pass program name, arguments of a command line and (export) environment variables to "execve" function. if the program is input with relative path name, it will search from ECE551PATH first;if found, extend it with absolute path name;if not found, print print error message to inform command not found and exit. 
 */
void command::execute() {
  size_t n = args.size();
  //we are not allowed to change args[i]
  //so args[i] has fixed size
  char * argv[1000];
  for (size_t i = 0; i < n; i++) {
    argv[i] = args[i];
  }
  argv[n] = NULL;

  //relative path name
  char temp[1000];
  temp[0] = '\0';
  if (argv[0][0] != '/') {  //program is in name of a relative path
    std::string absPath(argv[0]);
    read_paths(absPath);

    if (absPath != "") {
      size_t n = absPath.length();
      strncpy(temp, absPath.c_str(), n);
      temp[n] = '\0';
      argv[0] = temp;
    }
    else {  //if not found, print error message and exit
      fprintf(stderr, "Command %s not found\n", args[0]);
      exit(EXIT_FAILURE);
    }
  }

  struct stat sb;  //used to check is if the command is executable
  if (!(stat(argv[0], &sb) == 0 && sb.st_mode & S_IXUSR)) {
    fprintf(
        stderr,
        "Command %s not found\n",
        args[0]);  //it is better to say command is not executable, but based on the specification, we say not found here
    exit(EXIT_FAILURE);
  }

  n = envps.size();
  char * environ[1000];
  for (size_t i = 0; i < n; i++) {
    environ[i] = envps[i];
  }
  environ[n] = NULL;
  //print_info();
  //char * environ[] = {NULL};
  execve(argv[0], argv, environ);
  perror("execve");  // execve() retunrs only on error
  exit(EXIT_FAILURE);
}
/*
NAME
print_info -- print all information stored in a command object
DESRIPTION
1.arguments extracted from a command line
2.paths from ECE551PATH
3.environment variables passed down(export) to program 
 */
void command::print_info() {
  std::cout << args.size() << " command arguments: " << std::endl;
  args.print_all();
  std::cout << "ECE551PATH has paths: " << std::endl;
  cmd_paths.print_all();
  std::cout << envps.size() << " passed-down environment variables: " << std::endl;
  envps.print_all();
}
/*
NAME
read_paths -- search directory which may contains the program we are attempting to run
DESRIPTION
when a program is given with simply its relative path, the print_paths() function are invoked to up search arguments/paths stored in arguments "cmd_paths" which is actually initialized by "ECE551PATH" in this shell. Then, extend the program with an absolute path name
 */
void command::read_paths(std::string & program) {
  size_t numPaths = cmd_paths.size();
  bool found = false;
  for (size_t i = 0; (i < numPaths + 1) && (!found); i++) {
    std::string thePath;
    if (i != numPaths) {
      thePath = cmd_paths[i];
    }
    else {
      thePath = cdir;
    }
    DIR * dir = opendir(thePath.c_str());
    if (dir == NULL) {
      perror("");
      //dont' closedir(dir) here or it will give you warning,-Werror==nonnull
      //since argument of closedir can't be NULL
    }
    else {
      struct dirent * ent;
      while (((ent = readdir(dir)) != NULL) && (!found)) {  //read files under directory "dir"
        std::string file_name(ent->d_name);
        if (file_name == "." || file_name == "..") {
          continue;
        }
        if (file_name == args[0]) {  //if program is found, extend it with absolute path
          program = thePath + std::string("/") + file_name;
          found = true;
        }
      }
      closedir(dir);
    }
  }
  if (!found) {
    program = "";
  }
}

/*
NAME
read_path()
 */
