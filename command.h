#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>

#include "arguments.h"

class command
{
 private:
  //fields
  arguments args;       //command arguements
  arguments cmd_paths;  //paths extracted from ECE551PATH
  arguments envps;      //environment variables
  std::string cdir;     //current directory

  const char * read_cdir();
  void read_paths(std::string & file_name);

 public:
  //constructors
  command() {}
  explicit command(const char * line,
                   const char * path,
                   const var_val & expts,
                   const std::string & _cidr) :
      args(line, ' ', '\n'),
      cmd_paths(path, ':', '\0'),
      envps(expts),
      cdir(_cidr) {}  //methods
  void execute();     //execute the command
  void print_info();  //print command arguments, ECE551PATH, and exported environment variables

  //destructor
  ~command() {}
};
