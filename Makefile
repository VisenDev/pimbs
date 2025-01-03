CLANG=clang
COSMOCC=~/c/cosmocc/bin/cosmocc
SRCDIR=src
BUILDDIR=build
TESTFILE=run_tests.c
TESTEXE=$(BUILDDIR)/test
CFLAGS =-std=c89 \
		-pedantic \
		-g \
		-Wextra \
		-Wall \
		-Wfloat-equal \
		-Wundef \
		-Wshadow \
		-Wcast-align \
		-Wstrict-prototypes \
		-Wwrite-strings \
		-Wstrict-overflow=5 \
		-Wcast-qual \
		-Wswitch-enum \
		-Wconversion \
		-Wunreachable-code \
		-Wpointer-arith \
		-Wuninitialized \
		-Winit-self \
		-Wdouble-promotion \
		-Wformat \
		-Wformat-security \
		-Wnull-dereference \
		-Wunused \
		-Wimplicit-fallthrough \
		-Wunused-result \
		-Wno-unused-parameter \
		-Wold-style-definition \
		-Wredundant-decls \
		-Wnested-externs \
		-Wmissing-include-dirs 
TEST_CFLAGS= \
		$(CFLAGS) \
		-Wno-missing-prototypes \
		-Wno-unsafe-buffer-usage \
		-Wno-padded \
		-Weverything \
		-Werror \
		-fsanitize=address \
		-fsanitize=undefined \
		-ferror-limit=1 




test: $(TESTFILE) $(SRCDIR)/*h 
	@mkdir -p $(BUILDDIR)
	$(CLANG) -DUSE_STDLIB=1 $(TESTFILE) $(TEST_CFLAGS) -o $(TESTEXE)
	./$(TESTEXE)

nostdlib: $(TESTFILE) $(SRCDIR)/*h 
	@mkdir -p $(BUILDDIR)
	$(CLANG) -DUSE_STDLIB=0 $(TESTFILE) $(CFLAGS) -nostdlib -lSystem -o $(TESTEXE) 
	./$(TESTEXE)

# Rule to build and run tests
portable: 
	@mkdir -p $(BUILDDIR)
	$(COSMOCC) -DUSE_STDLIB=1 $(TESTFILE) $(CFLAGS) -Werror -fno-sanitize=address -fno-sanitize=undefined -o $(TESTEXE) 
	./$(TESTEXE)

# Clean rule
.PHONY: clean
clean: 
	trash $(BUILDDIR)
