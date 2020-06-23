#include "server.h"

server::server(int server_socket, int port)
  :server_socket(server_socket){
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);
  bind(server_socket, (SA*)&server_addr, sizeof(server_addr));
  listen(server_socket, 10 /*BACKLOG*/);
  mutex = PTHREAD_MUTEX_INITIALIZER;
  condition_var = PTHREAD_COND_INITIALIZER;
  for(int i = 0; i < MAX_CLIENTS; i++) {
    pthread_create(&thread_pool[i], NULL, thread_function, this);
  }
}

//-----------------------------------------------------------------------------

void* server::handle_connection(void* pclient_socket) {
  int client_socket = *((int*)pclient_socket);
  free(pclient_socket);
  char buffer[4096];
  int bytes_read;
  // Sending welcome message to client.
  std::string welcome = "Welcome to the chat session!\n";
  welcome += "Please enter your name : ";
  write(client_socket, &welcome[0], strlen(welcome.c_str()));
  // Receive the name.
  memset(buffer, 0, sizeof(buffer));
  bytes_read = read(client_socket, buffer, sizeof(buffer) -1);
  buffer[bytes_read] = '\0';
  socketToUserNameMap[client_socket] = std::string(buffer);
  std::cout << "Client name : " << std::string(buffer) << std::endl;
  //Tell others who arrived.
  std::string msg = "## " + std::string(buffer) + " has joined this session!";
  std::list<int>::iterator it;
  for(it = client_sockets_list.begin();
      it != client_sockets_list.end();
      ++it) {
    write(*it, &msg[0], msg.length());
  } 
  while(true) {
    memset(buffer, 0, sizeof(buffer));
    if((bytes_read = read(client_socket,
                          buffer,
                          sizeof(buffer) - 1)) > 0) {
      buffer[bytes_read] = '\0';
      std::cout << "Message received from client socket "
                << client_socket
                << " : "
                << std::string(buffer)
                << std::endl;
      std::cout.flush();
      std::string buffString = std::string(buffer);
      //If exit message then tell others who exited.
      if(buffString == ":EXIT") {
        std::string exitMsg = "## " + socketToUserNameMap[client_socket];
        exitMsg += " has left the session!";
        for(it = client_sockets_list.begin();
            it != client_sockets_list.end();
            ++it) {
          if(*it == client_socket) {
            continue;
          }
          write(*it, &exitMsg[0], exitMsg.length());
        }
        break;
      }
      // Sending to all clients.
      std::string sendMsg(buffer);
      sendMsg = socketToUserNameMap[client_socket] + " : " + sendMsg;
      for(it = client_sockets_list.begin();
          it != client_sockets_list.end();
          ++it) {
        if(*it == client_socket) {
          continue;
        }
        write(*it, &sendMsg[0], sendMsg.length());
      }
    }
  }
  client_sockets_list.remove(client_socket);
  close(client_socket);
  return NULL; 
}

//-----------------------------------------------------------------------------

void* startServer(void* arg) {
  server* serverObj = (server*)arg;
  std::cout << "Server at socket number "
            << serverObj->server_socket
            << " started!"
            << std::endl;
  SA_IN client_addr;
  while(true) {
    socklen_t addr_size = sizeof(SA_IN);
    int client_socket = accept(serverObj->server_socket, 
                           (SA*)&client_addr,
                           (socklen_t*)&addr_size);
    pthread_mutex_lock(&(serverObj->mutex));
    std::string clientIP(inet_ntoa(client_addr.sin_addr));
    std::cout << "Server at socket no : "
              << serverObj->server_socket
              << " connected to client at IP : "
              << clientIP
              << " socket : "
              << client_socket
              << std::endl;
    serverObj->client_sockets_list.push_back(client_socket);
    // Allot a thread to the client.
    int *pclient = (int*)malloc(sizeof(int));
    *pclient = client_socket;
    serverObj->conn_queue.push(pclient);
    // Signal that a client has arrived.
    pthread_cond_signal(&(serverObj->condition_var));
    pthread_mutex_unlock(&(serverObj->mutex));
  }
  return NULL;
}

//-----------------------------------------------------------------------------

void* thread_function(void* arg) {
  server* serverObj = (server*)arg;
  int* pclient;
  while(true) {
    pclient = NULL;

    pthread_mutex_lock(&(serverObj->mutex));
    if(serverObj->conn_queue.size() == 0) {
      // The thread waits until it is signaled to wake up. The lock (mutex) is
      // also released meanwhile, and when signaled the thread first acquires
      // lock and then only returns here to continue execution.
      pthread_cond_wait(&(serverObj->condition_var), 
                        &(serverObj->mutex));
    }
    // Now there is a request in the queue to be handled.
    pclient = serverObj->conn_queue.front();
    serverObj->conn_queue.pop();
    pthread_mutex_unlock(&(serverObj->mutex));

    if(pclient != NULL) {
      serverObj->handle_connection(pclient);
    }
  }
}

//-----------------------------------------------------------------------------

