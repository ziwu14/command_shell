###### COMPONENTS 

---

This program is composed of main program "shell.cpp", class files "command.h", "command.cpp", "arguments.h", "arguments.cpp", "envArgs.cpp", and 2 simple programs for specific test cases "print_args", "endless_loop" 

**USAGE** 

----

After compilation, a program "myShell" is created. Running the program, we have a bunch of operations to implement 

> 1.exit/EOF 
>
> 2.do nothing 
>
> 3.change current working directory, and update the prompt: we can run program under current directory without absolute path name 
>
> 4.manipulate environment variables: 
>
> **\$ set var val** @ $var=value 
>
> **\$ set var** @ var="" 
>
> **\$ set** @ print all var=val 
>
> **\$ export var1 var2 var3** @ pass down specified variables to child programs 
>
> **\$ inc var** @ increment 1 to the value of var. if the var is not set yet or the val is bot in base-10 number, then overwrite the value to 0, i.e. result would be 1 
>
> **\$ env** @ print all exported var=val 
>
> 5.execute programs with absolute path, programs with relative path name but found in paths of **ECE551PATH** environment variable, or programs with relative path name under current directory Notice if you run programs in other directory and not under ECE551PATH, you need to absolute path name, because my shell wouldn't translate ./ and ../ into corresponding directories 
>
> 6.after setting var=val, a command with character '\$'s, would be translated by a lookup table of exported environment variables, and therefore, a real command line is generated. The translation is recursive but \$s must be contiguous, 
>
> e.g. 
>
> set a ls 
>
> set b a 
>
> set c \$a 
>
> when we type "set", it would show "a=ls,b=a,c=ls". If we type "$$c", it would show "ls". 

**OTHERS** 

---

at the begining of shell.cpp, there is a "testing options" region, we could uncomment some #define lines for some specific test case purpose. 

> \#define cmd_args 
>
> //uncomment this will not allow execution of a program but print all information of a command object, including command arguments with their lengths, paths in ECE551PATH, exported environment variables 
>
> \#define line_translation 
>
> //uncomment this will not allow execution of a program, and is used to compare a command line before and after translation. Update environment variables is available 
>
> You can test the shell with some test cases list in the TESTCASE file

