#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <stdlib.h>

#define MAX 3

pthread_mutex_t mutex;
int clients[MAX]; //we hold the client sockets here
int n=0;
int simpleSocket;
pthread_t threads[MAX]; //threads to read each client
char buffer[270]; //write the messages here

void* readClient(void* sock) {
	int clientSocket = *((int* )sock);
	while (1) {
        if (read(clientSocket, buffer, sizeof(buffer)) > 0) {
            int i;
            pthread_mutex_lock(&mutex);
            for (i = 0; i < n; i++) {
                write(clients[i],buffer,sizeof(buffer));
            } //send the message to all clients including the current
            bzero(buffer, sizeof(buffer));
            pthread_mutex_unlock(&mutex);
        }
    }
}
int main() {
    struct sockaddr_in simpleServer;
    simpleServer.sin_family = AF_INET; //assign ip port
    simpleServer.sin_port = htons(2222);
    simpleServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    simpleSocket = socket( AF_INET , SOCK_STREAM, 0 );
    if (simpleSocket == -1) { //check if socket creation is successful
        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    }
    fprintf(stderr, "Socket created!\n");
    if (bind( simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer)) == -1) { //bind server and socket
        fprintf(stderr, "Could not bind to address!\n");
        close(simpleSocket);
        exit(1);
    }
    fprintf(stderr,"Bind completed!\n");
    if (listen( simpleSocket ,5 ) == -1) { //listen
        fprintf(stderr, "Cannot listen on socket!\n");
        close(simpleSocket);
        exit(1);
    }
    //accept client messages
    while (1) {
        int simpleChildSocket = accept(simpleSocket, (struct sockaddr *)NULL,NULL); 
        if (n==MAX) {
            char message[]="Capacity is full!";
            write(simpleChildSocket,message,sizeof(message));
            close(simpleChildSocket);
            continue;
        }
        clients[n]= simpleChildSocket;
        n++;
        //creating a thread for each client
        if (pthread_create(&threads[n],NULL,readClient,&simpleChildSocket) != 0){
            char message[]="An error occured!";
            write(simpleChildSocket,message,sizeof(message));
            clients[n]= NULL;
            n--;
            close(simpleChildSocket);
        }
    }
    close(simpleSocket);
    return 0;

}
