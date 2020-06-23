#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>

#include <iostream>
#include <string>
#include <list>
#include <queue>
#include <map>

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void* startServer(void* arg);
void* thread_function(void* arg);

#define MAX_CLIENTS 10

class server {
  public:
    server(int server_socket, int port);
    void* handle_connection(void* pclient_socket);
    int server_socket;
    SA_IN server_addr;
    pthread_mutex_t mutex;
    pthread_cond_t condition_var;
    std::queue<int*> conn_queue;
    std::list<int> client_sockets_list;
    std::map<int, std::string> socketToUserNameMap;
    pthread_t thread_pool[MAX_CLIENTS];
};

