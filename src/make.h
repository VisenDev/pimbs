


typedef struct {
    int argc;
    char ** argv;
} Make;

Make make_init(int argc, char ** argv) {
    Make result = {0};
    result.argc = argc;
    result.argv = argv;
}


int target(Make m, const char * name, const char * dependencies) {
    
}





int main(int argc, char ** argv) {
    Make m = make_init(argc, argv);

    while(!m.done) {
        if(target(m, "main.o", "main.c")) {
            system("cc main.c -c -o main.o");
        } else if(target(m, "main", "main.o")) {
            system("cc main.o -o main");
        } else if(target(m, "run", "main")) {
            system("main");
        } else if(target(m, "clean", "")) {
            system("rm *.o");
        }
    }

}



#include <stdio.h>
#include <stdlib.h>

void fatal_error(char * error_message) {
    fprintf(stderr, error_message);
    exit(1);
}

/*static int os_argc;
static char ** os_argv = NULL;*/
static char ** make_target = NULL;
static char make_incomplete = 1;

void init_make(int argc, char** argv) {
    if(argc != 2) {
        fatal_error("Wrong amount of cli arguments provided");
    } else {
        make_target = argv[1];
    }
}

void target_internal(char * name, char * dependencies) {
    
}

#define make \
    while(make_incomplete)

int main(int argc, char** argv) {
    make {
        target("test", "main") {
            
        }
    }
}
