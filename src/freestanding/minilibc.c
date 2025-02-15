int main();

void _start(void) {

    main();
}

#define PUTCHAR_FUNCTION putchar_syscall

#if 0
void putchar_syscall(char c) {
    __asm__ volatile (
        "mov $0x2000004, %%rax;"  /* SYS_write*/
        "mov $1, %%rdi;"          /* STDOUT_FILENO */
        "mov %0, %%rsi;"          /* Pointer to character */
        "mov $1, %%rdx;"          /* Length */
        "syscall;"
        : /* No output operands*/
        : "r"(&c) /* Input operand*/
        : "rax", "rdi", "rsi", "rdx", "memory"
    );
}
#endif

#if defined(__APPLE__)
// macOS (x86_64) implementation using inline assembly and the macOS syscall convention.
void putchar_syscall(char c) {
    __asm__ volatile (
        "mov $0x2000004, %%rax;\n" // SYS_write = 0x2000000 + 4
        "mov $1, %%rdi;\n"         // File descriptor 1 (stdout)
        "mov %0, %%rsi;\n"         // Address of character to print
        "mov $1, %%rdx;\n"         // Write 1 byte
        "syscall;\n"
        : // no output
        : "r"(&c)
        : "rax", "rdi", "rsi", "rdx", "memory"
    );
}

#elif defined(__linux__)
// Linux (x86_64) implementation using inline assembly and the Linux syscall convention.
void putchar_syscall(char c) {
    __asm__ volatile (
        "mov $1, %%rax;\n"   // SYS_write = 1
        "mov $1, %%rdi;\n"   // File descriptor 1 (stdout)
        "mov %0, %%rsi;\n"   // Address of character to print
        "mov $1, %%rdx;\n"   // Write 1 byte
        "syscall;\n"
        : // no output
        : "r"(&c)
        : "rax", "rdi", "rsi", "rdx", "memory"
    );
}

#elif defined(_WIN32)
#include <windows.h>
#if defined(_MSC_VER)
// Windows implementation using Microsoft inline assembly (MSVC, 32-bit)
void putchar_syscall(char c) {
    DWORD written;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    __asm {
        push 0            // lpOverlapped = NULL
        lea eax, written  // Address to store number of bytes written
        push eax          // &written
        push 1            // nNumberOfBytesToWrite = 1
        lea eax, c        // Address of character c
        push eax          // lpBuffer = &c
        push hStdOut      // hFile = standard output handle
        call WriteFile    // Call Windows API WriteFile
        add esp, 20       // Clean up the 5 arguments from the stack (5*4 bytes)
    }
}
#else
// Windows implementation using GCC-style inline assembly (e.g. MinGW, 32-bit)
void putchar_syscall(char c) {
    DWORD written;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    __asm__ __volatile__ (
        "pushl $0\n"                         "\n\t" // lpOverlapped = NULL
        "leal %[written], %%eax\n"           "\n\t" // get address of 'written'
        "pushl %%eax\n"                      "\n\t" // push &written
        "pushl $1\n"                         "\n\t" // push nNumberOfBytesToWrite = 1
        "leal %[c], %%eax\n"                 "\n\t" // get address of 'c'
        "pushl %%eax\n"                      "\n\t" // push lpBuffer = &c
        "pushl %[hStdOut]\n"                 "\n\t" // push hFile = standard output handle
        "call WriteFile\n"                   "\n\t" // call WriteFile
        "addl $20, %%esp\n"                  "\n\t" // clean up the stack (5 arguments * 4 bytes)
        : [written] "=m" (written)
        : [c] "m" (c), [hStdOut] "r" (hStdOut)
        : "eax"
    );
}
#endif

#else
#error "Unsupported platform"
#endif


void *memcpy(void *dest, const void *src, long n) {
    char *d = dest;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void *memset(void *s, int c, long n) {
    char *p = s;
    while (n--) {
        *p++ = (char)c;
    }
    return s;
}
