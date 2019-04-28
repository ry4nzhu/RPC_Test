//
//  server.cpp
//  RPC_Test
//
//  Created by 祝瑞洋 on 2019/4/27.
//  Copyright © 2019 ryanzhu. All rights reserved.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <array>
#include <iostream>

using namespace std;

struct Node {
    string s;
    struct Node* Next;
};

struct Node* head;
int socketfd;

void append(int connection,int size, char c) {
    for (int i = 0; i < size; ++i) {
        // first, receive the number of bytes for each node value
        int node_size = 0;
        recv(connection, &node_size, sizeof(int), MSG_WAITALL);
        node_size = ntohs(node_size);
        char* val = new char[node_size + 1];
        recv(connection, val, node_size, MSG_WAITALL);
        // append
        val[node_size - 1] = c;
        val[node_size] = '\0';
        // send back the appended value
        send(connection, val, node_size + 1, 0);
    }
}

int main(int argc, char* argv[]) {
    int server_port;
    if (argc != 2) {
        cout << "error: usage: " << argv[0] << " <serverPort>\n";
        exit(1);
    }
    server_port = atoi(argv[1]);
    cout << "server port is " << server_port << endl;
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    // (3) Create a sockaddr_in struct for the proper port and bind() to it.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(server_port);
    bind(socketfd, (struct sockaddr*) &addr, sizeof(addr));
    socklen_t length = sizeof(addr);
    if (getsockname(socketfd, (sockaddr *) &addr, &length) == -1) {
        perror("Error getting port of socket");
        return -1;
    }
    cout << "server listening to port " << ntohs(addr.sin_port) << endl;
    listen(socketfd, 10);
    while (true) {
        int connectionfd = accept(socketfd, 0, 0);
        cout << "connected" << endl;
        int sz = 0;
        recv(connectionfd, &sz, sizeof(sz), MSG_WAITALL);
        sz = ntohs(sz);
        cout << sz << endl;
        char append_char;
        recv(connectionfd, &append_char, 1, MSG_WAITALL);
        append(connectionfd, sz, append_char);
        close(connectionfd);
    }
    return 0;
}
