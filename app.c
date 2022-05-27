#ifdef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#define _CRT_SECURE_NO_WARNINGS

#include "lib_loader.h"
#include <WinSock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"user32.lib")

#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1)







typedef struct ip_hdr
{
	unsigned char ip_header_len:4; // 4-bit header length (in 32-bit words) normally=5 (Means 20 Bytes may be 24 also)
	unsigned char ip_version :4; // 4-bit IPv4 version
	unsigned char ip_tos; // IP type of service
	unsigned short ip_total_length; // Total length
	unsigned short ip_id; // Unique identifier

	unsigned char ip_frag_offset :5; // Fragment offset field

	unsigned char ip_more_fragment :1;
	unsigned char ip_dont_fragment :1;
	unsigned char ip_reserved_zero :1;

	unsigned char ip_frag_offset1; //fragment offset

	unsigned char ip_ttl; // Time to live
	unsigned char ip_protocol; // Protocol(TCP,UDP etc)
	unsigned short ip_checksum; // IP checksum
	unsigned int ip_srcaddr; // Source address
	unsigned int ip_destaddr; // Source address
} IPV4_HDR;

typedef struct udp_hdr
{
	unsigned short source_port; // Source port no.
	unsigned short dest_port; // Dest. port no.
	unsigned short udp_length; // Udp packet length
	unsigned short udp_checksum; // Udp checksum (optional)
} UDP_HDR;

// TCP header
typedef struct tcp_header
{
	unsigned short source_port; // source port
	unsigned short dest_port; // destination port
	unsigned int sequence; // sequence number - 32 bits
	unsigned int acknowledge; // acknowledgement number - 32 bits

	unsigned char ns :1; //Nonce Sum Flag Added in RFC 3540.
	unsigned char reserved_part1:3; //according to rfc
	unsigned char data_offset:4; /*The number of 32-bit words in the TCP header.
	This indicates where the data begins.
	The length of the TCP header is always a multiple
	of 32 bits.*/

	unsigned char fin :1; //Finish Flag
	unsigned char syn :1; //Synchronise Flag
	unsigned char rst :1; //Reset Flag
	unsigned char psh :1; //Push Flag
	unsigned char ack :1; //Acknowledgement Flag
	unsigned char urg :1; //Urgent Flag

	unsigned char ecn :1; //ECN-Echo Flag
	unsigned char cwr :1; //Congestion Window Reduced Flag

	////////////////////////////////

	unsigned short window; // window
	unsigned short checksum; // checksum
	unsigned short urgent_pointer; // urgent pointer
} TCP_HDR;

typedef struct icmp_hdr
{
	BYTE type; // ICMP Error type
	BYTE code; // Type sub code
	USHORT checksum;
	USHORT id;
	USHORT seq;
} ICMP_HDR;

IPV4_HDR *iphdr;
TCP_HDR *tcpheader;
UDP_HDR *udpheader;
ICMP_HDR *icmpheader;

struct sockaddr_in src,dst;

// FILE *f;
FILE *alert;







void clear(){
    system("cls");
}

void file_setup(){
    char *tmp;
    if(get_content("setting.txt",GET_STR,&tmp) != 1){
        printf("[>>] Create setting.txt ...\n");
        put_content("setting.txt",PUT_STR,"127.0.0.1");
    }

    if(get_content("dataset.txt",GET_STR,&tmp) != 1){
        printf("[>>] Create dataset.txt ...\n");
        put_content("dataset.txt",PUT_STR,"username\npassword");
    }

    // int code = fopen_s(&f,"log.txt","a");
    // if(code != 0){
    //     printf("[>>] Cannot write log.txt - %d",code);
    //     exit(0);
    // }
}










void report(char *src,char *dst,char *proto,char *text,char *data){
    int code = fopen_s(&alert,"alert.txt","a");
    if(code != 0){
        printf("[>>] Failed to open alert.txt - %d\n",code);
        fclose(alert);
        report(src,dst,proto,text,data);
    }

    time_t t = time(NULL);
    struct tm tm;
    localtime_s(&tm,&t);

    fprintf(alert,"\n###################################################################################################\n");
    fprintf(alert,"Time                : %02d-%02d-%d %02d:%02d:%02d\n",tm.tm_mday,tm.tm_mon+1,tm.tm_year+1900,tm.tm_hour,tm.tm_min,tm.tm_sec);
    fprintf(alert,"Source IP           : %s\n",src);
    fprintf(alert,"Destination IP      : %s\n",dst);
    fprintf(alert,"Protocol            : %s\n",proto);
    fprintf(alert,"Description         : Contain '%s'\n",text);
    fprintf(alert,"Data                : \n");
    fprintf(alert,"\t");
    for(int i = 0; i < strlen(data); i++){
        fprintf(alert,"%c",data[i]);
        if(i != 0){
            if(i % 50 == 0){
                fprintf(alert,"\n\t");
            }
        }
    }
    fprintf(alert,"\n###################################################################################################\n");
    fclose(alert);
}

void scanner(char *text,char *srcip,char *dstip,char *proto){
    char *raw;
    get_content("dataset.txt",GET_STR,&raw);

    char *delim = "\n";    

    int tlen = strlen(raw);
    int dlen = strlen(delim);

    int x = 0;
    int block_counter = 0;
    int start = 0;

    for(int i = 0; i < tlen; i++){
        if(raw[i] == delim[x]){
            if(x >= dlen - 1){
                char *block = malloc(block_counter);
                int last_char = 0;
                for(int j = 0; j < block_counter; j++){
                    block[j] = raw[start + j];
                    last_char++;
                }

                block[last_char] = '\0';

                // Code Here !!
                // Split item: block

                if(in_string(text,block) == 1){
                    report(srcip,dstip,proto,block,text);
                }

                //End Code;

                start = i + 1;
                block_counter = 0;
            }else{
                x++;
            }
        }else{
            block_counter++;
            x = 0;
        }
    }

    if(block_counter > 0){
        char *block = malloc(block_counter);
        int last_char = 0;
        for(int j = 0; j < block_counter; j++){
            block[j] = raw[start + j];
            last_char++;
        }

        block[last_char] = '\0';

        // Code Here !!
        // Split item: block

        if(in_string(text,block) == 1){
            report(srcip,dstip,proto,block,text);
        }

        //End Code;
    }
}

void place_data(char *data,int size,char *proto, char *src,char *dst){
    char a , c;
    char *d = malloc(size);
	int j;

    char *text = "";

	// fprintf(f,"\t|");
	for(int i=0 ; i < size ; i++)
	{
		c = data[i];
		a = ( c >= 32 && c <= 127) ? (unsigned char) c : '.';
		d[i] = a;
	}
    // fprintf(f,"{%s}",d);
    text = cat(text,d);
	// fprintf(f , "\n");
    scanner(text,src,dst,proto);
}



void ipHeader_proc(char *buf,int size,char **srcip,char **dstip){
    // time_t t = time(NULL);
    // struct tm tm;
    // localtime_s(&tm,&t);

    char *src_addr = malloc(16);
    char *dst_addr = malloc(16);

    src.sin_addr.s_addr = iphdr->ip_srcaddr;
    dst.sin_addr.s_addr = iphdr->ip_destaddr;

    InetNtop(AF_INET,(void *)&(src.sin_addr),src_addr,16);
    InetNtop(AF_INET,(void *)&(dst.sin_addr),dst_addr,16);

    *srcip = src_addr;
    *dstip = dst_addr;

    // iphdr = (IPV4_HDR*) buf;
    // fprintf(f,"[%02d-%02d-%d %02d:%02d:%02d] ",tm.tm_mday,tm.tm_mon+1,tm.tm_year+1900,tm.tm_hour,tm.tm_min,tm.tm_sec);
}


void tcp_dump(char *buf,int size){
    iphdr = (IPV4_HDR*) buf;
    unsigned short iphdrlen = iphdr->ip_header_len * 4;
    tcpheader = (TCP_HDR*) (buf+iphdrlen);

    char *srcip;
    char *dstip;

    ipHeader_proc(buf,size,&srcip,&dstip);

	// fprintf(f," TCP | %s:%u -> %s:%u",srcip,ntohs(tcpheader->source_port),dstip,ntohs(tcpheader->dest_port));


    place_data(buf+iphdrlen+sizeof(TCP_HDR),(size - sizeof(TCP_HDR) - iphdr->ip_header_len*4),"TCP",srcip,dstip);
}

void udp_dump(char *buf,int size){
    iphdr = (IPV4_HDR*)buf;
    unsigned short iphdrlen = iphdr->ip_header_len * 4;
    udpheader = (UDP_HDR*) (buf+iphdrlen);

    char *srcip;
    char *dstip;

    ipHeader_proc(buf,size,&srcip,&dstip);

    // fprintf(f," UDP | %s:%u -> %s:%u",srcip,ntohs(udpheader->source_port),dstip,ntohs(udpheader->dest_port));

    place_data(buf+iphdrlen+sizeof(UDP_HDR),(size - sizeof(UDP_HDR) - iphdr->ip_header_len*4),"UDP",srcip,dstip);
}



void packet_selection(char *buf,int size){
    iphdr = (IPV4_HDR*) buf;
    if(iphdr->ip_protocol == 6){
        tcp_dump(buf,size);
    }else if(iphdr->ip_protocol == 17){
        udp_dump(buf,size);
    }
}


void sniff(SOCKET s){
    int bit;
    char *buf = (char*)malloc(1024 * 50);

    if(buf == NULL){
        printf("BUFFER ERROR WHILE SNIFF\n");
        exit(0);
    }

    do{
        bit = recvfrom(s,buf,1024 * 50,0,0,0);
        if(bit > 0){
            packet_selection(buf,bit);
        }
    }while(bit > 0);
    free(buf);
}

int main(){
    clear();

    file_setup();

    char *ip;
    get_content("setting.txt",GET_STR,&ip);

    int err_sock;
    SOCKET s = sock_create(AF_INET,SOCK_RAW,IPPROTO_IP,ip,0,&err_sock);
    char *msg = err_check(err_sock);

    printf("[>>] Socket has been created\n");
    if(msg != NULL){
        printf("[>>] %s\n",msg);
        exit(0);
    }

    int j = 1, i = 1;
    if(WSAIoctl(s,SIO_RCVALL,&j,sizeof(j),0,0,(LPDWORD) &i,0,0) != 0){
        printf("[>>] WSAIoctl error\n");
        exit(0);
    }
    printf("[>>] Sniffer ready\n");
    ShowWindow(FindWindowA("ConsoleWindowClass",NULL),SW_HIDE);

    while(1){
        __try{
            sniff(s);
            sock_close(s);
        }__except(EXCEPTION_EXECUTE_HANDLER){
            printf("\n[>>] SEGMENTATION FAULT\n");
        }
    }
    exit(0);
    return 0;
}