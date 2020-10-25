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
pthread_t send_t, recv_t, login_t;
char msg[500];
char username[100];
char res[600];
char ip[INET_ADDRSTRLEN];
int len;
char name[12];

int connect_client_to_server();

void server_response_handler();

void start_chat();

void *client_handler(void *sock);

void save_user_name();

void show_chat_menu();

void format_sender();

int connect_client_to_server()
{
	portno = 8888;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(their_addr.sin_zero, '\0', sizeof(their_addr.sin_zero));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(portno);
	their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(my_sock, (struct sockaddr *)&their_addr, sizeof(their_addr)) < 0)
	{
		perror("connection not established");
		exit(1);
	}

	inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);
	printf("connected to %s", ip);
	server_response_handler();

	return 0;
}

void server_response_handler()
{
	bool flag = true;

	while (flag)
	{
		memset(msg, '\0', sizeof(msg));
		recv(my_sock, msg, 500, 0);

		switch (msg[0])
		{
		case logged_in_successfully:
		{
			std::cout << "\n\x1B[32mLogged In Successfully..." << std::endl;
			memset(msg, '\0', sizeof(msg));
			len = recv(my_sock, msg, 500, 0);
			save_user_name();
			show_chat_menu();
			start_chat();
			flag = false;
		}
		break;
		case invalid_id_password:
		{
			std::cout << "\n\033[1;31mInvalid UserId or Password" << std::endl;
			continue;
		}
		break;
		case exit_client:
		{
			std::cout << "\n\033[1;31mThank You!!!" << std::endl;
			flag = false;
			continue;
		}
		break;
		case already_registered:
		{
			std::cout << "\n\033[1;31mUser Is Already Registered. Try With Different User Id..." << std::endl;
			continue;
		}
		break;
		case already_logged_in:
		{
			std::cout << "\n\033[1;31mUser Is Already Logged In. Try Again..." << std::endl;
			continue;
		}
		break;
		case registered_successfully:
		{
			std::cout << "\n\x1B[32mRegistered Successfully And Logged In..." << std::endl;
			memset(msg, '\0', sizeof(msg));
			len = recv(my_sock, msg, 500, 0);
			save_user_name();
			show_chat_menu();
			start_chat();
			flag = false;
		}
		break;
		case already_registered_logged_in:
		{
			std::cout << "\n\033[1;31mUser Is Already Registered And Logged In. Try Again..." << std::endl;
			continue;
		}
		}

		std::cout << msg;
		memset(msg, '\0', sizeof(msg));
		fgets(msg, 500, stdin);
		write(my_sock, msg, strlen(msg));
	}
}

void start_chat()
{
	pthread_create(&recv_t, NULL, client_handler, &my_sock);
	while (fgets(msg, 500, stdin) > 0)
	{
		format_sender();
		len = write(my_sock, res, strlen(res));
		if (len < 0)
		{
			perror("message not sent");
			exit(1);
		}
		memset(msg, '\0', sizeof(msg));
		memset(res, '\0', sizeof(res));
	}
	pthread_join(recv_t, NULL);
	close(my_sock);
}

void *client_handler(void *sock)
{
	int client_sock = *((int *)sock);
	char msg[500];
	int len;
	int count = 0;
	while ((len = recv(client_sock, msg, 500, 0)) > 0)
	{
		msg[len] = '\0';
		fputs(msg, stdout);
		memset(msg, '\0', sizeof(msg));
	}
}

void save_user_name()
{
	strcpy(name, msg);
}

void show_chat_menu()
{

	std::cout << "\nWelcome To Chatify, " << name << std::endl;
	std::cout << "\n\x1B[33m------------CHAT MENU-------------\n"
			  << "|Online Users[@online]           |\n|Single Chat[@chat user_id msg]  |\n|Chat With All[@all msg]         |\n"
			  << "--------------ENTER---------------\n\n\x1B[34m";
}

void format_sender()
{
	strcat(res, name);
	strcat(res, ": ");
	strcat(res, msg);
}