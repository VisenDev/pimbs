CFLAGS=$(shell cat CFLAGS)
DEPENDENCIES=
SRCDIR=src
CFILES= #${SRCDIR}/allocator.c #${SRCDIR}/testing.c
CC=clang
BUILDDIR=build
TESTFILE=${SRCDIR}/run_tests.c
TESTEXE=${BUILDDIR}/test

# Generate object files list
OBJFILES=$(CFILES:${SRCDIR}/%.c=${BUILDDIR}/%.o)

# Default target
all: ${OBJFILES}

# Rule to compile each C file into an object file
${BUILDDIR}/%.o: ${SRCDIR}/%.c
	@mkdir -p ${BUILDDIR}
	${CC} -c $< -o $@ ${CFLAGS} ${DEPENDENCIES}

${TESTEXE}: all ${TESTFILE}
	${CC} ${TESTFILE} ${OBJFILES} ${CFLAGS} ${DEPENDENCIES} -o ${TESTEXE}

# Rule to build and run tests
.PHONY: test
test: ${TESTEXE} 
	time ./${TESTEXE}

# Clean rule
.PHONY: clean
clean:
	trash ${BUILDDIR}
