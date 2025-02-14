CLANG=clang
TCC=tcc
GCC=gcc-14
COSMOCC=~/c/cosmocc/bin/cosmocc
SRCDIR=src
BUILDDIR=build
TESTFILE=run_tests.c
TESTEXE=$(BUILDDIR)/test

CFLAGS= -std=c89 \
		-g \
		-Werror

WARNING_FLAGS= \
		-pedantic \
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

DEBUGGING_FLAGS= \
		-fsanitize=address \
		-fsanitize=undefined \

CLANG_FLAGS= -ferror-limit=1 

GCC_FLAGS= -fmax-errors=1

FREESTANDING_FLAGS= \
		-ffreestanding \
		-nostdlib \
        -fno-stack-protector \
		-static \
		-DNO_STDLIB



		#-Wno-missing-prototypes \
		-Wno-unsafe-buffer-usage \
		-Wno-padded \
		-Wno-used-but-marked-unused \
		-Weverything \
# -nostartfiles \
		-e _main \

#-DUSE_STDLIB=1 

test: #$(TESTFILE) $(SRCDIR)/*h 
	@mkdir -p $(BUILDDIR)
	$(CLANG) $(TESTFILE) $(CFLAGS) $(WARNING_FLAGS) $(CLANG_FLAGS) $(DEBUGGING_FLAGS) -o $(TESTEXE)
	./$(TESTEXE)

nostdlib: #$(TESTFILE) $(SRCDIR)/*h 
	@mkdir -p $(BUILDDIR)
	$(CLANG) $(TESTFILE) $(CFLAGS) $(FREESTANDING_FLAGS) -e __start -o $(TESTEXE) 
	./$(TESTEXE)

portable: 
	@mkdir -p $(BUILDDIR)
	$(COSMOCC) $(TESTFILE) $(CFLAGS) $(WARNING_FLAGS) -o $(TESTEXE) 
	./$(TESTEXE)

tcc:
	@mkdir -p $(BUILDDIR)
	$(TCC) $(CFLAGS) $(WARNING_FLAGS) $(TESTFILE) -o $(TESTEXE) 
	./$(TESTEXE)

linux:
	@mkdir -p $(BUILDDIR)
	$(CLANG) $(TESTFILE) $(CFLAGS) $(FREESTANDING_FLAGS) -o $(TESTEXE) --target=x86_64-freestanding-linux -fuse-ld=lld
	./$(TESTEXE)

# Clean rule
.PHONY: clean
clean: 
	trash $(BUILDDIR)
