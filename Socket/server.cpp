#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <iostream>
#include<thread>

#define PORT 8080 

void sendMsgs(int sock) {
    int valread = 0;
    char msg[1024]; 

    while(true){
        memset(msg, 0, sizeof msg);
        std::cin.getline(msg, 1024);
        send(sock ,msg ,strlen(msg) ,0); 
    }
}


void recvMsgs(int sock) {
    int valread = 0;
    char buffer[1024] = {0}; 

    while(true){
        memset(buffer, 0, sizeof buffer);
        valread = read(sock, buffer, 1024); 
        printf("Client: %s\n", buffer ); 
    }
}


int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT , &opt, sizeof(opt)))
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
    
    if (bind(server_fd, (struct sockaddr *)&address, 
                                sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } else {
        std::cout << "Server started. Listening...." << std::endl;
    } 


    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                    (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } else {
        std::cout << "Server started. Accepting...." << std::endl;
    } 

    std::cout << "Connection Accepted. Reading....." << std::endl;

    std::thread send_thread (sendMsgs, new_socket);
    std::thread recv_thread (recvMsgs, new_socket);

    send_thread.join();
    recv_thread.join();
    
    close(server_fd);
    return 0; 
} 