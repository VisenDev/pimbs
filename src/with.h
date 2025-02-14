#define with_open_file(varname, filename) \
    FILE* varname = fopen(filename); \
    int varname##_i_ = 0; \
    for( varname##_i_= 0; varname##_i_ < 2; ++varname##_i_) \
        if(varname##_i_ == 1) { \
            close(varname); \
        } else 


with_open_file(srcfile, "src.c") {
    printf(fread(srcfile));

}



#define with_memory(type, varname, count) \
    type * varname = malloc(sizeof(type) * count); \
    int varname##_i_ = 0; \
    for( varname##_i_= 0; varname##_i_ < 2; ++varname##_i_) \
        if(varname##_i_ == 1) { \
            free(varname);
        } else 


with_memory(int, buf, 1000) {
    

}
