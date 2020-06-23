# Multi-client multi-group chat application.

The application has 3 main components -

1. Coordinator - It resides at a well-known address which is known to each of
   the clients in advance. It communicates via UDP. It supports the following
   operations :
   i)   Create a chat server for each new session.
   ii)  Maintain a directory for each of the sessions created.
   iii) Return the address of the chat server hosting the session requested by
        the client.

2. Chat Server - A multi-threaded server created for each chat session
   individually. It used TCP for communication with the clients. It is
   responsible for receiving messages from chat clients and then forwarding
   them to the other clients present in the session dedicated to the particular
   server.

3. Chat Client - An application which allows the user to start, join, chat, and
   leave a chat session. It first contacts the coordinator via UDP to get the
   address of the required chat server. Then it connects directly to the chat
   server via TCP and is then available to chat with the other members present
   in the session.
   The chat client works with 2 threads while chatting - the first thread to
   read the messages received from the chat server, the second thread to send
   messages to the chat server.

The application has 2 output files : coordinator.out and client.out.

coordinator.out is linked with the server.o file, and hence is responsible for
creating the chat servers on demand.

To run the coordinator application - `$make server` <br />
To run the client application - `$make client`
