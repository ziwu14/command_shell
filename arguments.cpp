#include "arguments.h"

#include <assert.h>
#include <stdio.h>

#include <iostream>

//helper functions for skipping spaces
const char * arguments::skipSpace(const char * line) {
  while ((*line) == ' ') {
    line = line + 1;
  }
  return line;
}
/*
NAME
make_argument -- make one argument string and store it in argv arrays
DESCRPITION
it is called by the constructor internally. The "head" pointer points to the head of a contrived argument in the command line, and the "tail" pointer points to the ' ' of the argument. vector "catn" is a container to tell where is '\ '
 */
void arguments::make_argument(const char *& head,
                              const char *& tail,
                              std::vector<const char *> & catn) {
  //realloc argv and malloc for the new argument

  argv = (char **)realloc(argv, (argc + 2) * sizeof(*argv));
  argv[argc + 1] = NULL;
  size_t sz = tail - head - catn.size();
  argv[argc] = (char *)malloc((sz + 1) * sizeof(char));
  argv[argc][0] = '\0';  //initialized;

  //in C++, there is no void pointer which is the return type of malloc and
  //realloc, we have to use casting to explicitly state the return type
  if (catn.size() == 0) {  //if there is no backslash-space
    strncat(argv[argc], head, sz);
  }
  else {  //single or multiple backslash-space, then replace them with a single spc
    for (size_t i = 0; i < catn.size(); i++) {
      strncat(argv[argc], head, catn[i] - head - 1);
      strncat(argv[argc], " ", 1);
      head = catn[i] + 1;
    }
    strncat(argv[argc], head, tail - head);
  }

  argv[argc][sz] = '\0';
  argc++;
  //update pointers and clear concatenation vector
  if (*tail != '\0') {  //without this condition, it will give memcheck error on valgrind
    head = tail + 1;
  }
  head = skipSpace(head);
  tail = head;
  catn.clear();
}

/*
parameterized constructor for "arguments" class
DESCRPITION
separate a "line" into arguments by specifying "delimiter" and the line terminator "input_end" 
 */
arguments::arguments(const char * line, const char delimiter, const char input_end) :
    argc(0),
    argv(NULL) {
  const char * head = line;  //point to the head of a new argument
  head = skipSpace(head);
  const char * tail = head;

  std::vector<const char *> catn;  //positins of backslash-spc, used to concatenate spcs
  //separate by delimiter, and record the position of '\ ' which is stored in catn
  while ((tail = strchr(tail, delimiter)) != NULL) {
    if ((*(tail - 1) == '\\') && (delimiter == ' ')) {
      catn.push_back(tail);
      tail++;
    }
    else {
      make_argument(head, tail, catn);
    }
  }

  //final argument
  tail = strchr(head, input_end);
  if (tail != head) {
    make_argument(head, tail, catn);
  }
}

/*
another parameterized constructor for "arguments" class
DESCRPITION
binding VAR and value to "VAR=value" to make arguments 
 */
arguments::arguments(const var_val & expt) : argc(expt.size()), argv(NULL) {
  argv = (char **)realloc(argv, (argc + 1) * sizeof(char *));
  argv[argc] = NULL;  //put a null at the end

  char ** ptr = argv;
  var_val::const_iterator it = expt.begin();
  for (; it != expt.end(); ++it) {
    std::string temp(it->first + "=" + it->second);
    size_t n = temp.size();
    *ptr = (char *)malloc((n + 1) * sizeof(char));
    strncpy(*ptr, temp.c_str(), n);
    (*ptr)[n] = '\0';  //null-terminated
    ptr++;
  }
}

/*
NAME
check_envCmd -- to check if a command line specifying any environment manipulation
DESCRPITION
4 operations for environment variables. return the operation # to tell which operation is read from the command line
 */
int arguments::check_envCmd() {
  if (strcmp(argv[0], "set") == 0) {
    return 0;
  }
  else if (strcmp(argv[0], "export") == 0) {
    return 1;
  }
  else if (strcmp(argv[0], "inc") == 0) {
    return 2;
  }
  else if (strcmp(argv[0], "env") == 0) {
    return 3;
  }
  else {
    return -1;
  }
}

/*
DESCRIPTION
ramdom access to argumens
 */
char * arguments::operator[](size_t index) const {
  assert(0 <= index && index < argc);
  return argv[index];
}

/*
NAME
print_all - print all arguments, one per line
DESCRIPTION
used for debug, by looking into all arguments with their corresponding lengths
 */
void arguments::print_all() const {
  for (size_t i = 0; i < argc; i++) {
    std::cout << "argument[" << i << "] is " << argv[i] << " in length of " << strlen(argv[i])
              << std::endl;
  }
}
/*
destructor
DESCRIPTION
free matches any malloc/realloc in constructors
 */
arguments::~arguments() {
  for (size_t i = 0; i < argc; i++) {
    free(argv[i]);
  }
  free(argv);
}
