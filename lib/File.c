#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// What's included:
//      - File Read
//      - File Write

#define GET_BYTE 1
#define GET_STR 2

#define PUT_BYTE 1
#define PUT_STR 1

int get_content(char *loc,int mode,char **buf){
    FILE *f;
    FILE *gs;

    char *m;
    if(mode == GET_BYTE){
        m = "rb";
    }else{
        m = "r";
    }

    if(fopen_s(&f,loc,m) == 0){
        if(fopen_s(&gs,loc,m) == 0){
            char c;
            int pos = ftell(f);
            fseek(f, 0, SEEK_END);
            int size = ftell(f);
            fseek(f, pos, SEEK_SET);
            char *str = malloc(size);

            int counter = 0;
            while(counter < size){
                c = fgetc(f);
                str[counter++] = c;
            }
            str[size] = '\0';
            *buf = str;
            fclose(f);
            fclose(gs);
            return 1;
        }
        return 0;
    }
    return 0;
}

int put_content(char *loc,int mode,char *data){
    FILE  *f;
    char *m;
    if(mode == PUT_BYTE){
        m = "wb";
    }else{
        m = "w";
    }

    if(fopen_s(&f,loc,m) == 0){
        int len = strlen(data);
        for(int i = 0; i < len; i++){
            fputc(data[i],f);
        }
        fclose(f);
        return 1;
    }
    return 0;
}