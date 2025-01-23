#include <stdio.h>
#include <string.h>
#include <assert.h>

#define type_cap 256

struct Serializer;
typedef void (*SerializeFn)(struct Serializer *, void * value);

typedef struct Serializer {
    char * outbuf;
    unsigned long outbuf_len;
    unsigned long outbuf_cap;
    
    SerializeFn type_fn[type_cap];
    const char * type_name[type_cap];
    long type_name_len[type_cap];
    long type_len;
} Serializer;

/*REGISTER FUNCTIONS*/
void serializer_register(Serializer * self, char * name, long name_len, SerializeFn fn) {
    long i = self->type_len;
    self->type_len += 1;
    assert(self->type_len < type_cap);

    self->type_fn[i] = fn;
    self->type_name[i] = name;
    self->type_name_len[i] = name_len;
}

void serializer_write(Serializer * self, char * bytes, unsigned long bytes_len) {
    assert(bytes_len < (self->outbuf_cap - self->outbuf_len));
    memcpy(&self->outbuf[self->outbuf_len], bytes, bytes_len); 
    self->outbuf_len += bytes_len;
}

/*SERIALIZATION FUNCTIONS*/
void serialize_int(Serializer * self, void * value) {
    char buf[128] = {0};
    unsigned long len = sprintf(buf, "%d", *(int*) value);
    serializer_write(self, buf, len);
}

void serialize_long(Serializer * self, void * value) {
    char buf[128] = {0};
    unsigned long len = sprintf(buf, "%ld", *(long*) value);
    serializer_write(self, buf, len);
}

void serialize_float(Serializer * self, void * value) {
    char buf[128] = {0};
    unsigned long len = sprintf(buf, "%f", *(float*) value);
    serializer_write(self, buf, len);
}

void serialize_double(Serializer * self, void * value) {
    char buf[128] = {0};
    unsigned long len = sprintf(buf, "%lf", *(double*) value);
    serializer_write(self, buf, len);
}

/*INIT FN*/
Serializer serializer_init(char * outbuf, unsigned long outbuf_cap) {
   Serializer self = {0}; 
   self.outbuf = outbuf;
   self.outbuf_cap = outbuf_cap;

   serializer_register(&self, "int", 3, serialize_int); 
   serializer_register(&self, "long", 4, serialize_long); 
   serializer_register(&self, "float", 5, serialize_float); 
   serializer_register(&self, "double", 6, serialize_double); 
   return self;
}

/*serialize*/
#include <stdarg.h>
#include <stdio.h>
void serialize(Serializer * self, const char ** fmt, ...) {
    
}


typedef struct {
    int x;
    int y;
    float vel;
    float acc;
} Test;

int main() {
    char buf[4096] = {0};
    Test t = {0};
    Serializer s = serializer_init(buf, sizeof(buf));

    serializer_write(&s, "{\n", 2);

    serializer_write(&s, "x:", 2);
    serialize_int(&s, &t.x);

    serializer_write(&s, ", y:", 4);
    serialize_int(&s, &t.y);

    printf("%s", s.outbuf);

}
