#include <cstdio>
#include <cstdlib>

char* readFile(const char* fileName, int *fileSize){
    FILE *fp = fopen(fileName, "r");
    if(fp == nullptr){
        printf("Failed to open %s\n", fileName);
        exit(EXIT_FAILURE);
    }
    
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    *fileSize = size;
    rewind(fp);

    char* buffer = (char*)malloc(sizeof(char)*size);

    for(int i = 0; i < size; i++){
        buffer[i] = fgetc(fp);
    }

    fclose(fp);

    return buffer;//Don't forget to free
}