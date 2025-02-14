


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
