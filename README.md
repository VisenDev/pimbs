# The **P**reprocessor **I**s **M**y **B**uild **S**ystem

Pimbs is my personal collection of C data structure and utility libraries. It is
written as [stb](https://github.com/nothings/stb)-style single header libraries.
I am attempting to explore a few things in this project, Namely:
- Is there any satisfyingly ergonomic way to create generic data structures in C?
- An alternative way of creating single header libraries. 

Most single header libraries are create by have a HEADER and an IMPLEMENTATION
section of the header file. The HEADER section contains data structures and 
function prototypes. The IMPLEMENTATION section is only processed if a specific 
macro is defined, and implements the functions. 

Doing things in this way makes in convenient for users to use this library in 
thier projects, as all you need to do is copy the file into your project and 
`#include` it.

However, while this approach helps solve the problem of the build system and 
installation complexity headache that is usually involved when trying to use C
libraries in a project, there is still another problem: Header files and 
function prototypes fundamentally violate DRY -> Do not Repeat Yourself

When refactoring a functions signature in C, not only do you have to change the
functions implementation and all of its callsights. You also have to change the
function prototype defined in the header and make sure it is consistent with the
implementation's signature. This adds unnecessary complexity and complication. 
Ideally, a function's prototype should only need to be specified once. 

To rectify this, I am exploring the idea of defining functions like so

```C
int foo(bool bar, float biff, double bap)
#ifdef FOO_IMPLEMENTATION
{
    /*... function implementation ...*/
    return 0;
}
#else
;
#endif
```

Using this setup, if the file is included as a normal header, then 
`FOO_IMPLEMENTATION` is undefined. Thus, the preprocessor evalates the `#else`
branch instead of the function's body, leaving a semicolon - thereby forming
a valid function prototype.

After preprocessing:
```C
int foo(bool bar, float biff, double bap);
```

In the implementation file, the user can `#define FOO_IMPLEMENTATION` before 
including. Thus, the processor outputs this instead.

After preprocessing:
```C
int foo(bool bar, float biff, double bap)
{
    /*... function implementation ...*/
    return 0;
}
```

Thus, the function's signature need not be defined twice, and refactoring is
much simpler. The downside is that the `#ifdef #else #endif` add a bit of 
boilerplate, and it remains to be seen whether this is worth the cost. 

# Requirements to build
This project is written in strictly compliant ansi C (C89) with no dependencies,
not even the C standard library! Usage is as simple as copying the entire project
(or just needed files) into your project and `#include`'ing them (with the 
necessary `#define X_IMPLEMENTATION` macros defined when needed)

# Provided modules

- 'attributes.h' : Compiler agnostic common C attributes (such as NORETURN)
- 'debug.h' : Debugging functions, such as assert
- 'tui.h' : Colored ansi terminal output, requires a printf compatible `LOG_FUNCTION`
    to be defined to get actual output.
- 'testing.h' : Super simple unit testing setup. See runtests.c for example usage.
- 'vec.h' : Generic vector data structure
- 'list.h' : Generic linked list data structure
- 'sset.h' : Generic sparse set data structure
- 'hash.h' : Generic hashmap data structure (not fully completed)
- 'allocator.h' : Zig inspired allocator interface, provides an allocator interface
    and several common allocators, such as a leak-checking allocator, a logging
    allocator, an always failing allocator, a fixed buffer allocator, and an
    (optional) libc allocator (if desired)
- 'defer.h' : Super simple defer mechanism for C
- 'concat.h' : provides the common `CONCAT` macro
- 'bitmap.h' : Macros to create simple fixed sized bitmaps
- 'svec.h' : Macros to create simple generic fixed sized vectors
- 'strutils.h' : Provides several common string operation functions
- 'errors.h' : Defines standard error codes and their meanings when used in pimbs

There are also some experimental things I am working on in the `wip` directory. 
Like an ecs and various types of config file parsers.
