#######################################################################
#
#  -E stops compilation after precompiling
#  -S stops compilation after assembling
#  -c stops compilation after compiling (no link)
#
#  -v prints commands executed to run compiles
#######################################################################
CC				= clang
#COPTS			= -std=c99 -O3 -Wall
#COPTS			= -std=c99 -g -Wall
COPTS			= -g 
#CPU_OPTS		= -mcpu=powerpc64 -faltivec

# LIBS			= -lmisc -lm -lvga
LIBS			= -framework OpenCL

CFLAGS			= $(COPTS) $(CPU_OPTS)
ROOTNAME		= openclscan

#CFLAGS		= $(COPTS) -I$(XINCLUDE)
#LDFLAGS		= -L. 

SRC				= $(ROOTNAME).c
OBJS			= $(ROOTNAME).o
PROGS			= $(ROOTNAME)

#######################################################################

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
