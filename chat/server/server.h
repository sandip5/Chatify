#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "../dbConnection/db_operation.h"

#define port 8888
#define register_client '1'
#define login_client '2'
#define exit_client '3'
#define send_online_users_list "@online\n"
#define send_msg_to_specific_user_id "@chat"
#define send_msg_to_all_online_user "@all"

dboperation db_operation_obj;

int clients[100];
int number_of_client = 0;
struct sockaddr_in my_addr, their_addr;
int my_sock;
int client_sock;
socklen_t their_addr_size;
int portno;
pthread_t send_t, recv_t;
char msg[500];
int len;
char ip[INET_ADDRSTRLEN];
char list_of_online_user[500];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct client_info
{
    int sockfd;
    std::string user_id;
    std::string password;
    bool login_status = false;
    char ip[INET_ADDRSTRLEN];
};

std::vector<client_info> online_user;

int main();

void accept_client_request();

void start_chat_system();

void display_dashboard(client_info &cl);

void register_user(client_info &cl);

void login_user(client_info &cl);

bool is_user_logged_in(std::string userId, std::string password);

void *client_handler(void *sock);

void send_msg_to_all(std::string, char *, int);

void send_logged_user_name(client_info &cl);

void send_msg_to_one(std::string, char *, std::string);

void command_identifier(char *msg, int curr);

std::vector<std::string> recvd_msg_splitter(const std::string &client_response, std::string delimiter);

void make_online_user_list();