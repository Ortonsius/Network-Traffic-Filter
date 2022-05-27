#include "lib/File.c"
#include "lib/net.c"
#include "lib/stddata.c"

int get_content(char *loc,int mode,char **buf);
int put_content(char *loc,int mode,char *data);

char* err_check(int errcode);
SOCKET sock_create(int family,int type,int proto,const char* ip,int port,int *err);
void sock_close(SOCKET s);

void var_empty(var *v);
void var_set(var *a, int type, void *data);
void var_get(var v,void *buf);
void print_var(var data);
void array_add(Array *a,var data);
void array_ch(Array *a,var data,int index);
void array_del(Array *a,int index);
char *cat(char *left,char *right);
Array split(char *text,char *delim);
char *join(Array *arr,char delim[]);
int in_string(char *str,char* checked);