CC=g++
CFLAGS=-std=gnu++03 -pedantic -Wall -Werror  -O3
DBGFLAGS=-std=gnu++03 -pedantic -Wall -Werror -ggdb3 -DDEBUG

SRCS=$(wildcard *.cpp)
$(info    SRCS is $(SRCS))
_OBJS=$(patsubst %.cpp,%.o,$(SRCS))
OBJS=$(addprefix obj/,$(_OBJS))
$(info    OBJS is $(OBJS))
_DBGOBJS=$(patsubst %.cpp,%.dbg.o,$(SRCS))
DBGOBJS=$(addprefix obj/,$(_DBGOBJS))
$(info    DBGOBJS is $(DBGOBJS))

.PHONY: clean depend all

all: myShell myShell-debug
myShell: $(OBJS)
	g++ -o $@ -O3 $(OBJS)
myShell-debug: $(DBGOBJS)
	g++ -o $@ -ggdb3 $(DBGOBJS)

obj/%.o:%.cpp
	g++ $(CFLAGS) -c -o $@ $<
obj/%.dbg.o:%.cpp
	g++ $(DBGFLAGS) -c -o $@ $<

clean:
	rm -f *.o  *~ myShell myShell-debug obj/*.o  
depend:
	makedepend $(SRCS)
	makedepend -a -o .dgb.o $(SRCS)
# DO NOT DELETE

command.o: command.h /usr/include/stdio.h /usr/include/stdlib.h
command.o: /usr/include/string.h /usr/include/unistd.h
command.o: /usr/include/features.h /usr/include/stdc-predef.h arguments.h
command.o: /usr/include/dirent.h
envArgs.o: /usr/include/string.h arguments.h /usr/include/stdlib.h
arguments.o: arguments.h /usr/include/stdlib.h /usr/include/string.h
arguments.o: /usr/include/assert.h /usr/include/features.h
arguments.o: /usr/include/stdc-predef.h /usr/include/stdio.h
shell.o: /usr/include/limits.h /usr/include/pwd.h /usr/include/features.h
shell.o: /usr/include/stdc-predef.h /usr/include/stdio.h
shell.o: /usr/include/stdlib.h /usr/include/string.h /usr/include/unistd.h
shell.o: command.h arguments.h

command.dgb.o: command.h /usr/include/stdio.h /usr/include/stdlib.h
command.dgb.o: /usr/include/string.h /usr/include/unistd.h
command.dgb.o: /usr/include/features.h /usr/include/stdc-predef.h arguments.h
command.dgb.o: /usr/include/dirent.h
envArgs.dgb.o: /usr/include/string.h arguments.h /usr/include/stdlib.h
arguments.dgb.o: arguments.h /usr/include/stdlib.h /usr/include/string.h
arguments.dgb.o: /usr/include/assert.h /usr/include/features.h
arguments.dgb.o: /usr/include/stdc-predef.h /usr/include/stdio.h
shell.dgb.o: /usr/include/limits.h /usr/include/pwd.h /usr/include/features.h
shell.dgb.o: /usr/include/stdc-predef.h /usr/include/stdio.h
shell.dgb.o: /usr/include/stdlib.h /usr/include/string.h
shell.dgb.o: /usr/include/unistd.h command.h arguments.h
