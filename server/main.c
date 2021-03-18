#include <stdio.h>
#include <stdlib.h> // atoi
#include <string.h> // memset ...
#include <unistd.h> // sockaddr_in, read, write ...
#include <arpa/inet.h> // htnol, htons, INADDR_ANY, sockaddr_in ...
#include <sys/socket.h>

#define PORT 4444
#define BIG_BUF 409600
#define SMALL_BUF 102400

void error_handling(char* message);

int main(int argc, char* argv[]) {
    int serv_sock;
    int clnt_sock;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr; // use in accept func
    socklen_t clnt_addr_size;

    // TCP, ipv4
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket error");
    }

    // clear address and set IP and port
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    // binding
    if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind error");
    }

    if (listen(serv_sock, atoi(argv[1])) == -1) {
        error_handling("listen error");
    }

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) {
        error_handling("accept error");
    }

    char input[SMALL_BUF] = {0,};
    char buf[BIG_BUF] = {0,};
    
    while (1) {
        if (read(clnt_sock, buf, sizeof(buf)-1) == -1) {
            error_handling("reading error");
        }

        printf("%s", buf);
        scanf("%[^\n]s", input);

        write(clnt_sock, input, sizeof(input));

        memset(buf, 0, sizeof(buf));
        memset(input, 0, sizeof(input));
    }

    close(clnt_sock);
    close(serv_sock);

    return 1;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(-1);
}