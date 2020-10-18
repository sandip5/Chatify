#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>

#define logged_in_successfully '1'
#define invalid_id_password '2'
#define exit_client '3'
#define already_registered '4'
#define already_logged_in '5'
#define registered_successfully '6'
#define already_registered_logged_in '7'

struct sockaddr_in their_addr;
int my_sock;
int client_sock;
int their_addr_size;
int portno;
pthread_t send_t,recv_t, login_t;
char msg[500];
char username[100];
char res[600];
char ip[INET_ADDRSTRLEN];
int len;

int connect_client_to_server();

void server_response_handler();

void start_chat();

void *recv_msg(void *sock);

int connect_client_to_server()
{
	portno = 8888;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(their_addr.sin_zero,'\0',sizeof(their_addr.sin_zero));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(portno);
	their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(my_sock,(struct sockaddr *)&their_addr,sizeof(their_addr)) < 0) 
    {
		perror("connection not established");
		exit(1);
	}

	inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
	printf("connected to %s",ip);
	server_response_handler();
    
    return 0;
}


void server_response_handler()
{
	bool flag = true;

	while(flag)
	{
		memset(msg,'\0',sizeof(msg));
		recv(my_sock, msg, 500, 0);

        switch (msg[0])
        {
            case logged_in_successfully:
		        {
			        std::cout << "\nLogged In Successfully..." << std::endl;
			        start_chat();
                    flag = false;
		        }
                break;
		    case invalid_id_password:
		        {
			        std::cout << "\nInvalid UserId or Password" << std::endl;
		        }
                break;
		    case exit_client:
		        {
			        std::cout << "\nThank You!!!" << std::endl;
                    flag = false;
		        }
                break;
		    case already_registered:
		        {
			        std::cout << "\nUser Is Already Registered. Try With Different User Id..." << std::endl;
		        }
                break;
		    case already_logged_in:
		        {
			        std::cout << "\nUser Is Already Logged In. Try Again..." << std::endl;
		        }   
                break;
		    case registered_successfully:
		        {
			        std::cout << "\nRegistered Successfully And Logged In..." << std::endl;
			        start_chat();
			        flag = false;
		        }
                break;
		    case already_registered_logged_in:
		        {
			        std::cout << "\nUser Is Already Registered And Logged In. Try Again..." << std::endl;
		        }
        }
		
		std::cout << msg;
		memset(msg,'\0',sizeof(msg));
		fgets(msg, 500, stdin);
		write(my_sock, msg, strlen(msg));
	}
}

void start_chat()
{
	pthread_create(&recv_t, NULL, recv_msg, &my_sock);
	while(fgets(msg,500,stdin) > 0) 
    {
		strcpy(res, username);
		strcat(res,">>");
		strcat(res,msg);
		len = write(my_sock,res,strlen(res));
		if(len < 0) 
		{
			perror("message not sent");
			exit(1);
		}
		memset(msg,'\0',sizeof(msg));
		memset(res,'\0',sizeof(res));
	}
	pthread_join(recv_t,NULL);
	close(my_sock);
}

void *recv_msg(void *sock)
{
	int client_sock = *((int *)sock);
	char msg[500];
	int len;
	while((len = recv(client_sock,msg,500,0)) > 0) {
		msg[len] = '\0';
		fputs(msg,stdout);
		memset(msg,'\0',sizeof(msg));
	}
}



