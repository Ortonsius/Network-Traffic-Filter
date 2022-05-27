#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1)

#define SOCK_ERR_1 "WSAStartup Error"
#define SOCK_ERR_2 "Failed to build socket"
#define SOCK_ERR_3 "Failed to bind socket"

char* err_check(int errcode){
    if(errcode == 1){
        return SOCK_ERR_1;
    }else if(errcode == 2){
        return SOCK_ERR_2;
    }else if(errcode == 3){
        return SOCK_ERR_3;
    }else{
        return NULL;
    }
}   

SOCKET sock_create(int family,int type,int proto,const char* ip,int port,int *err){
    WSADATA ws;
    SOCKET s;
    struct sockaddr_in c;
    if(WSAStartup(MAKEWORD(2,2),&ws) != 0){
        *err = 1;
    }

    if((s = socket(family,type,proto)) != 0){
        *err = 2;
    }

    InetPton(family,ip,&c.sin_addr.s_addr);
    c.sin_family = family;
    c.sin_port = port;

    if(bind(s,(struct sockaddr *)&c,sizeof(c)) != 0){
        *err = 3;
    }

    *err = 0;
    return s;
}

void sock_close(SOCKET s){
    closesocket(s);
}