#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../cJSON/cJSON.h"

void* pthread_send(void *arg);
void send_to_clnt (int clnt_sockt);
int serv_socket_init(int serv_sock,char *conf_buf);
int load_conf(char *conf_buf);

char *response_head = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n";
char *content = "<!DOCTYPE html>\n"
                "<html lang=\"en\">\n"
                "  <body>\n"
                "    <h1>No Page Found!</h1>\n"
                "  </body>\n"
                "</html>";
char *address = NULL;
int port = 0;
char *root_dir = NULL;

void* pthread_send(void *arg)
{

    char buf[512];
    int clnt_sockt;
    clnt_sockt = *(int *)arg;
    recv(clnt_sockt,buf,512,0);
    if(!strncmp(buf,"GET / HTTP/1.1",14))
        send_to_clnt(clnt_sockt);
    printf("clnt_socket %d\n",clnt_sockt);
    close(clnt_sockt);
}

void send_to_clnt (int clnt_sockt){
    char send_buf[512] = {0};
    char file_buf[512] = {0};
    FILE * fd;
    strcat(send_buf,response_head);
    if(fd = fopen("index.html","r"))
    {
        while(fgets(file_buf,sizeof(file_buf),fd))
        {
            strcat(send_buf,file_buf);
            memset(file_buf,0, sizeof(file_buf));
        }
        fclose(fd);
    }
    else
    {
        strcat(send_buf,content);
    }
    write(clnt_sockt,send_buf, sizeof(send_buf));
    memset(send_buf,0, sizeof(send_buf));
    close(clnt_sockt);
    printf("send finish\n");
}

int serv_socket_init(int serv_sock,char *conf_buf)
{
    cJSON * cJSON_conf = NULL;
    cJSON * cJSON_http = NULL;
    cJSON * cJSON_root_dir = NULL;
    cJSON * cJSON_address = NULL;
    cJSON * cJSON_port = NULL;

    cJSON_conf = cJSON_Parse(conf_buf);
    if(!cJSON_conf)
    {
        printf("parse fail\n");
        exit(-1);
    }

    cJSON_root_dir = cJSON_GetObjectItem(cJSON_conf,"www-data");
    root_dir = cJSON_root_dir->valuestring;
    cJSON_http = cJSON_GetObjectItem(cJSON_conf,"http");
    cJSON_address = cJSON_GetObjectItem(cJSON_http,"address");
    address = cJSON_address->valuestring;
    cJSON_port = cJSON_GetObjectItem(cJSON_http,"port");
    port = cJSON_port->valueint;

    chdir(root_dir);
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(address);
    serv_addr.sin_port = htons(port);
    bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    return listen(serv_sock,20);
}

int load_conf(char *conf_buf)
{
    FILE *fd = NULL;
    char temp[512] = {0};
    if(fd= fopen("../day8/server.conf","r"))
    {
        while(fgets(temp,sizeof(temp),fd))
        {
            strcat(conf_buf, temp);
            memset(temp,0, sizeof(temp));
        }
        fclose(fd);
        return 1;
    }
    return 0;
}
int main() {


    char conf_buf[512] = {0};
    if(!load_conf(conf_buf))
    {
        printf("load_conf error\n");
        exit(-1);
    }

    int serv_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(serv_socket_init(serv_sock,conf_buf) != 0)
    {
        printf("serv_socket_init error\n");
        exit(-1);
    }

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size =sizeof(clnt_addr);
    int clnt_sockt;
    pthread_t tid;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(serv_sock,&readfds);


    while(1)
    {
        if(select(serv_sock+1,&readfds,NULL,NULL,NULL))
        {
            if(FD_ISSET(serv_sock,&readfds))
            {
                clnt_sockt = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
                if(clnt_sockt != -1)
                    pthread_create(&tid,NULL,pthread_send,&clnt_sockt);
            }
        }

    }
    close(serv_sock);
    return 0;
}
