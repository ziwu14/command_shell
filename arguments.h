#include <stdlib.h>
#include <string.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

typedef std::map<std::string, std::string>
    var_val;  //a map of string and string, serving as a variable-to-value dictonary

class arguments
{
 protected:
  //fields
  size_t argc;   //number of arguments
  char ** argv;  //array of strings to represent to arguments
  //helper functions
  const char * skipSpace(const char * line);  //a helper function to skip space
  void make_argument(
      const char *& head,
      const char *& tail,
      std::vector<const char *> &
          catn);  //a helper function to make one argument from a command line, which may be called multiple times until no arguments could be created

 public:
  //constructors
  arguments() : argc(0), argv(NULL) {}  //default constructor
  explicit arguments(
      const char * line,
      const char delimiter,
      const char
          input_end);  //parameterized constructor with input of command line, delimiter, and end character of the line
  explicit arguments(
      const var_val &
          expts);  //parameterized constructor with input of exported environment var-val pari, to make arguments in form of "VAR=value"
  //methods
  size_t size() const { return argc; }  //to get number of arguments
  int check_envCmd();  //check if the the line contains operations which update environment variables
  char * operator[](size_t index) const;  //to get random access element
  void print_all() const;                 //print arguments stored in the object
  //destructor
  virtual ~arguments();
};

class envArgs
{
 private:
  var_val set;   //set but not export VAR=value pair are stored in a map<string,string>
  var_val expt;  //export VAR=value pair in a map<string,string>
  //4 environment manipulations, selected by public method update_envs
  void set_envs(const arguments & input);
  void export_envs(const arguments & input);
  void inc_envs(const arguments & input);
  void env_envs(const arguments & input);
  //3 helper functions for line translation
  void separate(const char * line, std::vector<std::string> & catn);
  void combine(std::vector<std::string> & catn, std::string & ans);

 public:
  envArgs() {}
  void lookup(std::string & var);
  void translate_line(
      char *&
          line);  //serve as a dictionary which is looked up to translate a command line which includes $var -> if not found, use empty string to replace it
  void update_envs(const arguments & input, int op);  //to update two set of environment variables
  var_val & pass_expt() {
    return expt;
  }  //pass the export environment variables, those are information required by a command execution
  ~envArgs() {}
};
