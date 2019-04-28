//
//  client.cpp
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
#include <vector>
#include <iostream>

using namespace std;

struct Node {
    string s;
    struct Node* Next;
};

struct Node* head;
int socketfd;

void append(Node* head, char c) {
    // should send every value in the linked list through internet, cannot send pointer values
    int size = 0;
    Node* temp = head;
    vector<size_t> each_length;
    while (temp != nullptr) {
        each_length.push_back(temp->s.length() + 2);
        temp = temp->Next;
        size++;
    }
    cout << size << endl;
    // first, send the overall size of the linked list to the server
    int size_n = htons(size);
    send(socketfd, &size_n, sizeof(size_n), 0);
    
    // second, send the character that append to the linked list to the server
    send(socketfd, &c, 1, 0);
    
    temp = head; // reset pointer temp
    for (int i = 0; i < size; ++i) {
        // send every node's value
        int sz = (int) each_length[i] - 1;
        sz = htons(sz);
        send(socketfd, &sz, sizeof(sz), 0);
        send(socketfd, temp->s.c_str(), temp->s.length() + 1, 0);
        char* rec = new char[each_length[i]];
        recv(socketfd, rec, each_length[i], 0);
        string new_val(rec);
        temp->s = new_val;
        temp = temp->Next;
        delete[] rec;
    }
}

int main(int argc, char* argv[]) {
    // Establish client socket
    char* server;
    int server_port;
    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);
    
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    struct hostent *host = gethostbyname(server);
    memcpy(&(addr.sin_addr), host->h_addr, host->h_length);
    addr.sin_port = htons(server_port);
    // connect to server
    if (connect(socketfd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        return -1;
    }
    
    head = new Node;
    // a remote procedual call that append char 'a' to every node in the list
    append(head, 'a');
    
    cout << head->s << endl;
    
    delete head;
    return 0;
}


