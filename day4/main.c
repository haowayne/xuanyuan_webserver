#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
char GET_BUF[512] = {0};

int main(int argc,char **argv) {
    printf("Hello, day4!\n");
    if (argc < 2)
    {
        printf("Usage: ./day4 URL");
        exit(1);
    }
    char *hostName = argv[1];
    struct hostent *host;

    if (host= gethostbyname(hostName))
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr = *((struct in_addr*)host->h_addr);
        serv_addr.sin_port = htons(80);
        connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

        strcat(GET_BUF,"GET / HTTP/1.1\r\n");
        strcat(GET_BUF,"Accept: html/text\r\n");
        char temp[512] = {0};
        sprintf(temp,"Host: %s\r\n",hostName);
        strcat(GET_BUF,temp);
        strcat(GET_BUF,"Connection: close\r\n\r\n");

        send(sock,GET_BUF, sizeof(GET_BUF),0);

        char buf_recv[1048];
        while(recv(sock,buf_recv,1048,0))
            printf("recv %s",buf_recv);
        close(sock);
        return 0;
    }


    return 0;
}
