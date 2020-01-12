#include <string.h>

#include "arguments.h"
/*
NAME 
translate_line -- used to translate a line containing '$' by replacing set environment $VAR with its corresponding value
DESCRIPTION
takes in command line "line", delimit it by '$', then look up the set environment variables dictionary to replace it with its corresponding value, 
and finally combine them together
 */

// a b
//   $a  $b.
//$a $b
//where a=ls   b=.
//ls $a $b
//$a b $b
//a b $
void envArgs::translate_line(char *& line) {
  //char * ptr = line;//ptr is used to store the start of the line, don't lose pointer "line"  because we are goint to reallocate it
  while (strchr(line, '$') != NULL) {
    std::vector<std::string> catn;  //used to stored separate strings
    //1.separate
    separate(line, catn);
    //2. combine
    std::string ans("");
    combine(
        catn,
        ans);  //the combine method is a helper function to translate each components and then concatenate them together
    size_t n = ans.length();
    line = (char *)realloc(line, (n + 1) * sizeof(char));
    line[n] = '\0';
    strcpy(line, ans.c_str());
  }
}
/*
NAME
separate -- separate a line to string components delimited by '$'
DESCRIPTION
input a command line "line", return a vector of strings "catn" which could concatene to the original string  
 */
void envArgs::separate(const char * line, std::vector<std::string> & catn) {
  while ((*line) == ' ') {  //skip spaces
    line++;
  }
  const char * head = line;
  const char * tail = head;
  std::string linep(head);  //c++ line, because std::string member functions are useful
  //1. delimit strings that are before $, and end character of the line
  //delimited by $
  while ((tail = strchr(tail, '$')) != NULL) {  //if $ is found
    if (tail > head) {                          //tail is after head
      catn.push_back(std::string(linep.begin() + (head - line), linep.begin() + (tail - line)));
      head = tail;
    }
    tail++;
  }

  //final argument
  tail = strchr(head, '\n');
  if (tail != NULL) {
    catn.push_back(std::string(linep.begin() + (head - line), linep.begin() + (tail - line + 1)));
  }
}

/*
NAME
combine -- 
DESCRIPTION
input a command line "line", return a vector of strings "catn" which could concatene to the original string  
 */
void envArgs::combine(std::vector<std::string> & catn, std::string & ans) {
  for (size_t i = 0; i < catn.size(); i++) {
    if (catn[i][0] == '$') {  //to avoid special case: no $ such as "a b " of "a b $c d"
      if (catn[i].size() == 1) {
        ans = ans + "$";
        continue;
      }
      size_t find_spc = catn[i].find(' ');  //to handle case such as "$a b$c"
      size_t find_nl = catn[i].find('\n');  //to handle case such as "$\n"
      std::string nonVar("");
      if (find_spc !=
          std::string::
              npos) {  //if space/new line is found, then split the string into two parts, Var part for look-up, non-Var part which will be appended to the new VAR part after look-up
        nonVar = catn[i].substr(find_spc);
        catn[i] = catn[i].substr(0, find_spc);
      }
      else if (find_nl != std::string::npos) {
        nonVar = "\n";
        catn[i] = catn[i].substr(0, find_nl);
      }
      lookup(catn[i]);
      catn[i] = catn[i] + nonVar;
    }
    ans = ans + catn[i];
  }
}
/*
NAME
lookup -- lookpu $VAR=value pari in set and replace $VAR with value 
DESCRIPTION
trasver the dictionary until find the corresponding translation. if not found, replace $VAR with empty string
the var must start with $
 */
void envArgs::lookup(std::string & var) {
  bool find_var = false;
  size_t pos;
  var_val::const_iterator it = set.begin();  //iterator through the map
  for (; it != set.end(); ++it) {
    if ((pos = var.find(it->first)) != std::string::npos) {
      if (pos == 1) {  //to avoid case such as "$abc" with bc=100
        var.replace(pos - 1, it->first.length() + 1, it->second);  //c++ string replace function
        find_var = true;
        break;
      }
    }
  }
  if (!find_var) {  //if not found in the map, then set it to empty
    var = "";
  }
}
/*
NAME
update_envs -- execute environment variable manipulations 
DESCRIPTION
Give the operation number, it will execute corresponding environment variable execution
 */
void envArgs::update_envs(const arguments & input, int op) {
  if (op == 0) {  //1. set var val
    set_envs(input);
  }
  else if (op == 1) {  //2. export var1 var2 ...
    export_envs(input);
  }
  else if (op == 2) {  //3. inc var
    inc_envs(input);
  }
  else if (op == 3) {  //4. env
    env_envs(input);
  }
}

/*
NAME
set_envs -- set but not export var 
DESCRIPTION
Usage: 
1."set var val" set VAR=value
2."set" prints all set environment variables
 */
void envArgs::set_envs(const arguments & input) {
  if (input.size() == 1) {
    var_val::const_iterator it = set.begin();
    for (; it != set.end(); ++it) {
      std::cout << it->first << "=" << it->second << std::endl;
    }
  }
  else if (input.size() == 2 || input.size() == 3) {
    std::string temp("");
    if (input.size() == 3) {
      temp = input[2];
    }

    var_val::iterator it_set = set.find(input[1]);  //to look for key from a map
    if (it_set != set.end()) {                      //if var found in set then overwrite it in set
      it_set->second = temp;
    }
    else {  //if not found in set then insert it to set
      set.insert(std::pair<std::string, std::string>(input[1], temp));
    }
  }
  else {
    std::cerr << "Usage:set var val" << std::endl;
  }
}
/*
NAME
expt_envs -- export var 
DESCRIPTION
Usage: export var1 var2 ...
4 cases is a combination of either var is found in set/expt or not 
 */
void envArgs::export_envs(const arguments & input) {
  for (size_t i = 1; i < input.size(); i++) {
    var_val::iterator it_set = set.find(input[i]);
    var_val::iterator it_expt = expt.find(input[i]);
    if (it_set != set.end()) {      //if found in set
      if (it_expt != expt.end()) {  //if found in expt then overwrite in expt
        it_expt->second = it_set->second;
      }
      else {  //if not found in expt then insert it to expt
        expt.insert(std::pair<std::string, std::string>(it_set->first, it_set->second));
      }
    }
    else {                          //if not found in set
      if (it_expt != expt.end()) {  //if found in expt then overwrite it with empty
        it_expt->second = "";
      }
      else {  //if not found in expt then insert it to expt with value empty
        expt.insert(std::pair<std::string, std::string>(input[i], ""));
      }
    }
  }
}

/*
NAME
inc_envs -- increment value of  var 
DESCRIPTION
Usage: inc VAR
Interprets the value of var as a number (base10), and increments it.
If var is not currently set, or does not represent a number
in base10, then the initial value is treated as 0 (and thus
incremented to 1)
 */
void envArgs::inc_envs(const arguments & input) {
  if (input.size() != 2) {
    std::cerr << "Usage: inc var" << std::endl;
  }
  else {
    var_val::iterator it_set = set.find(input[1]);
    if (it_set != set.end()) {  //found it in set
      bool is_number = true;
      for (size_t i = 0; i < it_set->second.size();
           i++) {  //traverse the string to see if any character is not 0-9
        if (!isdigit(it_set->second[i])) {
          is_number = false;
          break;
        }
      }
      if (!is_number) {
        it_set->second = "0";
      }
    }
    else {  //if not set yet, then insert it to the map with initial value 0
      set.insert(std::pair<std::string, std::string>(input[1], "0"));
    }
    //increment using stringstream
    it_set = set.find(input[1]);
    std::stringstream ss;
    int temp;
    ss << it_set->second;
    ss >> temp;
    ss.clear();
    temp++;
    ss << temp;
    ss >> it_set->second;
  }
}
/*
NAME
env_envs -- display all export environment variables VAR=value 
DESCRIPTION
Usage: env
4 cases is a combination of either var is found in set/expt or not 
 */
void envArgs::env_envs(const arguments & input) {
  if (input.size() != 1) {
    std::cerr << "Usage: env" << std::endl;
  }
  else {
    var_val::const_iterator it_expt = expt.begin();
    for (; it_expt != expt.end(); ++it_expt) {
      std::cout << it_expt->first << "=" << it_expt->second << std::endl;
    }
  }
}
