CFLAGSFILE=CFLAGS
CFLAGS=$(shell cat ${CFLAGSFILE})
SRCDIR=src
CC=clang
BUILDDIR=build
TESTFILE=run_tests.c
TESTEXE=${BUILDDIR}/test

${TESTEXE}: ${TESTFILE} ${SRCDIR}/*h ${CFLAGSFILE}
	mkdir -p ${BUILDDIR}
	${CC} ${TESTFILE} ${OBJFILES} ${CFLAGS} ${DEPENDENCIES} -o ${TESTEXE}

# Rule to build and run tests
.PHONY: test
test: ${TESTEXE} 
	./${TESTEXE}

# Clean rule
.PHONY: clean
clean:
	trash ${BUILDDIR}
