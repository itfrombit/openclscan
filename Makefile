ROOTNAME		= openclscan
LIBS			= -framework OpenCL

CC				= clang
COPTS			= -g 

#CC				= gcc
#COPTS			= -std=c99 -O3 -Wall
#COPTS			= -std=c99 -g -Wall

CFLAGS			= $(COPTS)

#######################################################################

SRC				= $(ROOTNAME).c
OBJS			= $(ROOTNAME).o
PROGS			= $(ROOTNAME)

default: $(PROGS)

$(ROOTNAME):		$(ROOTNAME).o
	$(CC) $(COPTS) $(LIBS) -o $(ROOTNAME) $(ROOTNAME).o

$(ROOTNAME).o:	$(ROOTNAME).c
	$(CC) -c $(COPTS) $(ROOTNAME).c -o $(ROOTNAME).o

preproc: $(ROOTNAME).c
	gcc -o $(ROOTNAME).i -E $(ROOTNAME).c

clean:
	rm -f *.a *.o *.i $(ROOTNAME)

strip:
	strip $(PROGS)
	ls -l $(PROGS)

all:	clean default strip

#######################################################################
