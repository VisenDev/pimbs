CFLAGSFILE=CFLAGS
CFLAGS=$(shell grep -v '\#' $(CFLAGSFILE) )
SRCDIR=src
CC=clang
BUILDDIR=build
TESTFILE=run_tests.c
TESTEXE=$(BUILDDIR)/test


all: $(TESTFILE) $(SRCDIR)/*h $(CFLAGSFILE) builddir
	$(CC) -DUSE_STDLIB=1 $(TESTFILE) $(CFLAGS) -o $(TESTEXE)

$(TESTEXE):
	@echo "Please build $(TESTEXE) using either 'make all' or 'make nostdlib'."
	@exit 1

builddir:
	@mkdir -p $(BUILDDIR)

nostdlib: $(TESTFILE) $(SRCDIR)/*h $(CFLAGSFILE) builddir
	$(CC) -DUSE_STDLIB=0 $(TESTFILE) $(CFLAGS) -nostdlib -lSystem -fno-sanitize=address -fno-sanitize=undefined -o $(TESTEXE) 

# Rule to build and run tests
.PHONY: test
test: $(TESTEXE)
	./$(TESTEXE)

# Clean rule
.PHONY: clean
clean: 
	trash $(BUILDDIR)
