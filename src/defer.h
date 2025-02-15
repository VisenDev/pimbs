#ifndef DEFER_H
#define DEFER_H

#define defer(label) \
    while(0) \
        while(1) \
            if (1) { \
                goto label##_done_; \
            } else label:

#define deferred(label) do { goto label; label##_done_:; } while (0)

#endif




#if 0
int toggle() {
    static int state = 0; // Static variable retains value between function calls
    return state++ < 2;   // Increments and returns true for the first two calls
}

while(toggle())
    if(i == 1) {
        /*free memory*/
    } else {
        /*use memory*/
    }

#define later \
    while(toggle())

#define first else

int main() {
    char * mem = malloc(100);

    later {
        free(mem);
    } first {
        int i = 0; 
        for(i = 0; i < 100; ++i) {
            
        }
    }

#endif

#if 0
typedef enum {
    LATCH_GET,
    LATCH_TOGGLE,
    LATCH_ON,
    LATCH_OFF,
} LatchCommand;

int latch(LatchCommand cmd) {
    static int state = 0;
    if(cmd == LATCH_GET) {
        return state;
    } else if(cmd == LATCH_TOGGLE) {
        state = !state;
    } else if(cmd == LATCH_ON) {
        state = 1;
    } else if(cmd == LATCH_OFF) {
        state = 0; 
    } else {
        abort();
    }
}

#define after(statement) \
    for(latch(LATCH_ON); latch(LATCH_GET); latch(LATCH_OFF))

#define finally(statement) \
    for(;;(statement, 0)) mylabel: 
}

int main() {
    char * mem = malloc(100);
    finally(free(mem)) {
        int i = 0; 
        for(i = 0; i < 100; ++i) {
            printf("%c", mem[i]);    
        }
    }

}

#define eventually

#define butfirst

int main() {

    char * mem = malloc(100);
    eventually {
        free(mem);
    } butfirst {
        int i = 0; 
        for(i = 0; i < 100; ++i) {
            printf("%c", mem[i]);    
        }
    }

}
#endif
