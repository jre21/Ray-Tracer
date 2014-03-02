.KEEP_STATE:
SHELL=/bin/sh

#
#	define version of c compiler and linker
#
CC=	g++
LINK=	g++
#
#	define compiler flags
#
CFLAGS	= -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual \
	-Wcast-align -Wwrite-strings -fshort-enums -fno-common \
	-g -O3
LDLIBS	= -lm -lglut -lGLU -lGL

DEPS	= point.hh matrix.hh model.hh scene.hh view.hh surface.hh \
	mesh.hh sphere.hh mouse.hh frame_buffer.hh

ODIR	= obj
_OBJ	= main.o point.o matrix.o model.o scene.o view.o surface.o \
	mesh.o sphere.o mouse.o frame_buffer.o
OBJ	= $(patsubst %,$(ODIR)/%,$(_OBJ))

BIN	= viewer.bin

GENERATED = $(OBJ) $(BIN)

.PHONY	:	all
all	:	$(BIN)

.PHONY	:	run
run	:	$(BIN)
	MESA_DEBUG=1 ./$^ -gldebug

$(BIN)	:	$(OBJ)
	$(LINK) -o $@ $^ $(CFLAGS) $(LDLIBS)

.PHONY	:	objs
objs	:	$(OBJ)

$(ODIR)/%.o : %.cc %.hh $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY	:	gdb
gdb	:	$(BIN)
	gdb ./$(BIN)

.PHONY	:	valgrind
valgrind :	$(BIN)
	valgrind --trace-children=yes --leak-check=full \
		--show-reachable=yes ./$(BIN)

.PHONY	:	clean
clean	:
	-rm -f $(OBJ)

.PHONY	:	distclean
distclean :
	-rm -f $(GENERATED)
