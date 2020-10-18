#include "server.h"

int main()
{
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	their_addr_size = sizeof(their_addr);

	if (bind(my_sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0)
	{
		perror("binding unsuccessful");
		exit(1);
	}

	if (listen(my_sock, 5) != 0)
	{
		perror("listening unsuccessful");
		exit(1);
	}

	accept_client_request();

	close(my_sock);
	return 0;
}

void accept_client_request()
{
	while (1)
	{
		if ((client_sock = accept(my_sock, (struct sockaddr *)&their_addr, &their_addr_size)) < 0)
		{
			perror("accept unsuccessful");
			exit(1);
		}

		start_chat_system();
	}
}

void start_chat_system()
{
	struct client_info cl;
	pthread_mutex_lock(&mutex);
	inet_ntop(AF_INET, (struct sockaddr *)&their_addr, ip, INET_ADDRSTRLEN);

	printf("%s connected\n", ip);
	sleep(1);
	cl.sockfd = client_sock;
	strcpy(cl.ip, ip);
	clients[number_of_client] = client_sock;
	number_of_client++;
	display_dashboard(cl);
	pthread_create(&recv_t, NULL, recv_msg, &cl);
	pthread_mutex_unlock(&mutex);
}

void display_dashboard(client_info &cl)
{
	bool flag = true;
	int len = 0;
	char message[100];
	char msg[500];

	while (flag)
	{
		memset(msg, '\0', sizeof(msg));
		strcpy(message, "\n\t   \033[1;31mWelcome To Chat Application\n1. Register\n2. Login\n3. Exit\nEnter  : \033[0m");
		send(cl.sockfd, message, strlen(message), 0);
		memset(msg, '\0', sizeof(msg));
		len = recv(cl.sockfd, msg, 500, 0);
		switch (msg[0])
		{
		case register_client:
			register_user(cl);
			flag = false;
			break;
		case login_client:
			login_user(cl);
			flag = false;
			break;
		case exit_client:
		{
			memset(msg, '\0', sizeof(msg));
			strcpy(message, "3");
			send(cl.sockfd, message, strlen(message), 0);
			flag = false;
		}
		}
	}
}

void register_user(client_info &cl)
{
	bool flag = true;
	int len = 0;
	char message[100];
	char msg[500];
	std::string user_id, password;

	while (flag)
	{
		memset(msg, '\0', sizeof(msg));
		strcpy(message, "\n\033[1;31mEnter UserId : \033[0m");
		send(cl.sockfd, message, strlen(message), 0);
		len = recv(cl.sockfd, msg, 500, 0);
		msg[len] = '\0';
		user_id = msg;
		std::cout << user_id << std::endl;
		memset(msg, '\0', sizeof(msg));
		strcpy(message, "\n\033[1;31mEnter Password : \033[0m");
		send(cl.sockfd, message, strlen(message), 0);
		len = recv(cl.sockfd, msg, 500, 0);
		msg[len] = '\0';
		password = msg;
		std::cout << password << std::endl;
		if (is_user_logged_in(user_id, password))
		{
			strcpy(message, "7");
			send(cl.sockfd, message, strlen(message), 0);
			continue;
		}
		bool is_registered = true;
		if (is_registered)
		{
			cl.user_id = user_id;
			cl.password = password;
			cl.login_status = true;
			online_user.push_back(cl);
			flag = false;
			strcpy(message, "6");
			send(cl.sockfd, message, strlen(message), 0);
		}
		else
		{
			strcpy(message, "4");
			send(cl.sockfd, message, strlen(message), 0);
		}
	}
}

void login_user(client_info &cl)
{
	bool flag = true;
	int len = 0;
	char message[100];
	char msg[500];
	std::string user_id, password;

	while (flag)
	{
		sleep(1);
		memset(msg, '\0', sizeof(msg));
		strcpy(message, "\n\033[1;31mEnter UserId : \033[0m");
		send(cl.sockfd, message, strlen(message), 0);
		len = recv(cl.sockfd, msg, 500, 0);
		msg[len] = '\0';
		user_id = msg;
		std::cout << user_id << std::endl;
		memset(msg, '\0', sizeof(msg));
		strcpy(message, "\n\033[1;31mEnter Password : \033[0m");
		send(cl.sockfd, message, strlen(message), 0);
		len = recv(cl.sockfd, msg, 500, 0);
		msg[len] = '\0';
		password = msg;
		std::cout << password << std::endl;
		if (is_user_logged_in(user_id, password))
		{
			strcpy(message, "5");
			send(cl.sockfd, message, strlen(message), 0);
			continue;
		}
		std::cout << "               " << std::endl;
		bool is_user_registered = check_authentication(user_id, password);
		std::cout << "             " << std::endl;
		if (is_user_registered)
		{
			cl.user_id = user_id;
			cl.password = password;
			cl.login_status = true;
			online_user.push_back(cl);
			flag = false;
			strcpy(message, "1");
			send(cl.sockfd, message, strlen(message), 0);
			sleep(1);
			char greet[35];
			char name[12];
			strcpy(greet, "\x1B[33mWelcome, ");
			strcpy(name, cl.user_id.c_str());
			send(cl.sockfd, name, strlen(name), 0);
			strcat(greet, name);
			send(cl.sockfd, greet, strlen(greet), 0);
		}
		else
		{
			strcpy(message, "2");
			send(cl.sockfd, message, strlen(message), 0);
		}
	}
}

bool is_user_logged_in(std::string userId, std::string password)
{
	for (auto itr : online_user)
	{
		if (itr.user_id == userId && itr.password == password)
		{
			return true;
		}
	}

	return false;
}

bool check_authentication(std::string user_id, std::string password)
{
	if ((user_id == "sandip" || user_id == "kajal") && password == "123456")
	{
		std::cout << "Check server 77";
		return true;
	}

	return true;
}

void *recv_msg(void *sock)
{
	struct client_info cl = *((struct client_info *)sock);
	char msg[500];
	int len;
	int client_counter;
	int manipulate_client_counter;
	while ((len = recv(cl.sockfd, msg, 500, 0)) > 0)
	{
		msg[len] = '\0';
		printf("%s", msg);
		send_msg_to_all(msg, cl.sockfd);
		memset(msg, '\0', sizeof(msg));
	}
	pthread_mutex_lock(&mutex);
	printf("\033[1;31m%s disconnected\n", cl.ip);
	for (client_counter = 0; client_counter < number_of_client; client_counter++)
	{
		if (clients[client_counter] == cl.sockfd)
		{
			manipulate_client_counter = client_counter;
			online_user.erase(online_user.begin() + client_counter);
			while (manipulate_client_counter < number_of_client - 1)
			{
				clients[manipulate_client_counter] = clients[manipulate_client_counter + 1];
				manipulate_client_counter++;
			}
		}
	}
	number_of_client--;
	pthread_mutex_unlock(&mutex);
}

void send_msg_to_all(char *msg, int curr)
{
	int i;
	pthread_mutex_lock(&mutex);
	for (i = 0; i < number_of_client; i++)
	{
		if (clients[i] != curr)
		{
			if (send(clients[i], msg, strlen(msg), 0) < 0)
			{
				perror("sending failure");
				continue;
			}
		}
	}
	pthread_mutex_unlock(&mutex);
}