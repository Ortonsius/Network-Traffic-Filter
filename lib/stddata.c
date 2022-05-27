#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// What we need
// - String Method
// - Var implementation
// - Array implementation
//      + Method
//          - Array_add
//          - Array_del
//          - Array_edit


// ================================================================
// Var Implementation
// ================================================================

#define INT_VAL 1
#define FLOAT_VAL 2
#define DOUBLE_VAL 3
#define LONG_VAL 4
#define STRING_VAL 5
#define CHAR_VAL 6
#define VOID_VAL 7

typedef struct{
    int int_val;
    float float_val;
    double dbl_val;
    long long_val;
    char *str_val;
    char char_val;
    void *void_val;
    int type;
    // type == 1 ? int
    // type == 2 ? float
    // type == 3 ? double
    // type == 4 ? long
    // type == 5 ? string
    // type == 6 ? char
    // type == 7 ? void
} var;

void var_empty(var *v){
    var a;
    *v = a;
}

void var_set(var *a, int type, void *data){
    var_empty(a);
    if( type == INT_VAL ){              // INT
        a->int_val = (int)data;
        a->type = INT_VAL;
    }else if(type == FLOAT_VAL){        // FLOAT
        a->float_val = *(float*)data;
        a->type = FLOAT_VAL;
    }else if(type == LONG_VAL){         // LONG
        a->long_val = (long)data;
        a->type = LONG_VAL;
    }else if(type == DOUBLE_VAL){       // DOUBLE
        a->dbl_val = *(double*)data;
        a->type = DOUBLE_VAL;
    }else if(type == CHAR_VAL){         // CHAR
        a->char_val = (char)data;
        a->type = CHAR_VAL;
    }else if(type == STRING_VAL){       // STRING
        a->str_val = (char *)data;
        a->type = STRING_VAL;
    }else if(type == VOID_VAL){         // VOID
        a->void_val = data;
        a->type = VOID_VAL;
    }
}

void var_get(var v,void *buf){
    if( v.type == INT_VAL ){              // INT
        *(int*)buf = v.int_val;
    }else if(v.type == FLOAT_VAL){        // FLOAT
        *(float*)buf = v.float_val;
    }else if(v.type == LONG_VAL){         // LONG
        *(long*)buf = v.long_val;
    }else if(v.type == DOUBLE_VAL){       // DOUBLE
        *(double*)buf = v.dbl_val;
    }else if(v.type == CHAR_VAL){         // CHAR
        *(char*)buf = v.char_val;
    }else if(v.type == STRING_VAL){       // STRING
        *(char**)buf = v.str_val;
    }else if(v.type == VOID_VAL){         // VOID
        *(void**)buf = v.void_val;
    }
}

void print_var(var data){
    if(data.type == 1){
        printf("%d",data.int_val);
    }else if(data.type == 2){
        printf("%f",data.float_val);
    }else if(data.type == 3){
        printf("%f",data.dbl_val);
    }else if(data.type == 4){
        printf("%ld",data.long_val);
    }else if(data.type == 5){
        printf("%s",data.str_val);
    }else if(data.type == 6){
        printf("%c",data.char_val);
    }else if(data.type == 7){
        printf("%p",data.void_val);
    }
}














// ================================================================
// Array Implementation
// ================================================================

struct Arr{
    int len;
    var data[];
}newArray = {0,{}};

typedef struct Arr Array;


void array_add(Array *a,var data){
    a->data[a->len] = data;
    a->len++;
}

void array_ch(Array *a,var data,int index){
    a->data[index] = data;
}

void array_del(Array *a,int index){
    for(int i = index; i < a->len; i++){
        if(i + 1 >= a->len){
            a->len--;
        }else{
            a->data[i] = a->data[i+1];
        }
    }
}


























// ================================================================
// String Method
// ================================================================

char *cat(char *left,char *right){
    int llen = strlen(left);
    int rlen = strlen(right);
    
    char *res = malloc(llen + rlen);
    
    for(int i = 0; i < llen; i++){
        res[i] = left[i];
    }

    int jc = 0;
    for(int j = llen; j < llen + rlen; j++){
        res[j] = right[jc];
        jc++;
    }

    res[llen + rlen] = '\0';

    return res;
}

Array split(char *text,char *delim){
    Array arr = newArray;
    var data;

    int tlen = strlen(text);
    int dlen = strlen(delim);

    char *nd = malloc(dlen);
    nd = delim;

    char *nt = malloc(tlen);
    nt = text;

    int block_counter = 0;
    int start = 0;
    int counter = 0;

    int still_work = 1;
    int enb = 0;

    int asdla = 1;

    for(int i = 0; i < tlen; i++){
        if(nt[i] == nd[counter]){
            if(counter >= dlen - 1){
                char *block = malloc(block_counter);
                asdla++;
                int asdla = 0;
                for(int j = 0; j < block_counter; j++){
                    block[j] = nt[start + j];
                    asdla++;
                }

                block[asdla] = '\0';
                
                start = i + 1;
                block_counter = 0;

                var_set(&data,STRING_VAL,block);
                array_add(&arr,data);
                block = "";

                enb = 1;
                still_work = 0;
            }else{
                counter++;
                if(enb == 1) still_work = 1;
            }
        }else{
            block_counter++;
            if(enb == 1) still_work = 1;
        }
    }

    if(still_work == 1){
        char *block = malloc(block_counter);
        int asdla = 0;
        for(int j = 0; j < block_counter; j++){
            block[j] = nt[start + j];
            asdla++;
        }

        block[asdla] = '\0';
        var_set(&data,STRING_VAL,(void *)block);
        array_add(&arr,data);
        block = "";
    }

    return arr;
}


char *join(Array *arr,char delim[]){
    int dlen = strlen(delim);
    char *res = malloc(1);
    int res_size = 1;
    res = "\0";

    int at_start = 1;
    for(int i = 0; i < arr->len; i++){
        char *temp;
        var_get(arr->data[i],&temp);

        if(at_start == 0){
            temp = cat(delim,temp);
        }else{
            at_start = 0;
        }

        int bsize = strlen(temp);
        res_size += bsize;

        res = cat(res,temp);
    }
    res[res_size] = '\0';
    return res;
}

int in_string(char *str,char* checked){
    int slen = strlen(str);
    int clen = strlen(checked);

    int x = 0;

    for(int i = 0; i < slen; i++){
        if(str[i] == checked[x]){
            if(x >= clen - 1){
                return 1;
            }else{
                x++;
            }
        }else{
            x = 0;
        }
    }
    return 0;
}