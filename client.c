#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <stdlib.h>

char received[250];
pthread_t thread;
int simpleSocket=0;
//a thread that receives messages
void* receive(void *arg) {
    while (1) {
        if (read(simpleSocket, received, sizeof(received)) > 0) {
            fprintf(stderr,"%s\n", received);
            bzero(received, sizeof(received));
        }
    }
}
int main() {
    char name[20];
    char buffer[270];
    while (1) {
        fprintf(stderr,"Enter your name : \n");
        scanf("%s", name); //client enters the name before connecting to server
        if (strlen(name) > 20 || strlen(name) < 3) {
            fprintf(stderr,"Name should be 3-20 characters : \n");
            bzero(name, sizeof(name));
        }
        else {
            break;
        }
    }
    struct sockaddr_in simpleServer;
    simpleSocket = socket( AF_INET, SOCK_STREAM,0); //create socket
    if (simpleSocket == -1) { //check if socket creation is successful
        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    }
    simpleServer.sin_port = htons(2222); //assign ip port
    simpleServer.sin_family= AF_INET;
    simpleServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    if ((connect(simpleSocket ,(struct sockaddr *)&simpleServer,sizeof(simpleServer))) < 0) { //check connection
        fprintf(stderr, "Connection failed!\n");
        close(simpleSocket);
        exit(1);
    }
    strcpy(buffer, name); //announce that current user has joined the chat
    strcat(buffer, " has joined the chat");
    if (pthread_create(&thread,NULL,receive,NULL) != 0) {
        fprintf(stderr, "An error occured\n");
        close(simpleSocket);
        exit(1);
    }
    write(simpleSocket, buffer, sizeof(buffer));
    bzero(buffer, sizeof(buffer));
    fprintf(stderr,"Enter your message (250 characters at most) : \n");
    while (1) {
        char message[250];
        if (fgets(message,250,stdin) > 0) {
            strcpy(buffer, name); //clients name will be shown when a message is sent
            strcat(buffer, ":");
            strcat(buffer, message); //message will be clientname: message
            write(simpleSocket, buffer, sizeof(buffer));
            bzero(buffer, sizeof(buffer));
            bzero(message, sizeof(message));
        }
    }
    pthread_join(thread,NULL);
    close(simpleSocket);
    return 0;
}