#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


void send_code(char *send_buf,char *response_head,char *content,int clnt_sockt){
    strcat(send_buf,response_head);
    strcat(send_buf,content);
    write(clnt_sockt,send_buf, sizeof(send_buf));
    memset(send_buf,0, sizeof(send_buf));
    close(clnt_sockt);
    printf("send code finish\n");
}

void send_file(char *send_buf,char *response_head,char *file_buf,FILE *fd,int clnt_sockt){
    strcat(send_buf,response_head);
    while(fgets(file_buf,sizeof(send_buf),fd))
        strcat(send_buf,file_buf);
    write(clnt_sockt,send_buf, sizeof(send_buf));
    memset(send_buf,0, sizeof(send_buf));
    close(clnt_sockt);
    printf("send file finish\n");
    fclose(fd);
}
int main() {

    char *response_head = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n";
    char *content = "<!DOCTYPE html>\n"
                   "<html lang=\"en\">\n"
                   "  <body>\n"
                   "    <h1>This is my first page!</h1>\n"
                   "  </body>\n"
                   "</html>";
    char buf[512] = {0};
    char send_buf[512] = {0};
    char file_buf[512] = {0};
    FILE *fd = NULL;
    int serv_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    serv_addr.sin_port = htons(10008);
    bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    listen(serv_sock,20);
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size =sizeof(clnt_addr);
    int clnt_sockt;
    while(1)
    {
        clnt_sockt = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
        recv(clnt_sockt,buf,512,0);
        if(!strncmp(buf,"GET / HTTP/1.1",14))
        {
            if(NULL == (fd = fopen("../day5/index.html","r")))
            {
                send_code(send_buf,response_head,content,clnt_sockt);
                continue;
            }
            send_file(send_buf,response_head,file_buf,fd,clnt_sockt);
            continue;
        }
    }
    close(serv_sock);
    return 0;
}
