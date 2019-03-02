#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <thread>
#include <vector>
#include <pthread.h>
#include "Request.h"
#include "Response.h"

const char * logpath = "/var/log/erss/proxy.log";


using namespace std;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define BUFF_SIZE 65535

void add_log(string str){
    //pthread_mutex_lock(&lock);
    std::ofstream file;
    file.open(logpath,std::ios::app);
    file << str << endl;
    file.close();
    //pthread_mutex_unlock(&lock);
}

void connect_to_server(Request req, int & dest_fd){
    int status_n;
    struct addrinfo host_info_n;
    struct addrinfo *host_info_list_n;
    const char *hostname_n = req.host.c_str();
    string portstr = to_string(req.port);
    const char *port_n = portstr.c_str();

    memset(&host_info_n, 0, sizeof(host_info_n));
    host_info_n.ai_family = AF_UNSPEC;
    host_info_n.ai_socktype = SOCK_STREAM;

    status_n = getaddrinfo(hostname_n, port_n, &host_info_n, &host_info_list_n);
    if (status_n != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        return;
    }

    dest_fd = socket(host_info_list_n->ai_family,
                     host_info_list_n->ai_socktype,
                     host_info_list_n->ai_protocol);

    if (dest_fd == -1) {
        cout << "Error: cannot create socket" << endl;
        return;
    }
    else if(dest_fd == 0){
        cout << "Server socket not set" << endl;
        return;
    }

    status_n = connect(dest_fd, host_info_list_n->ai_addr, host_info_list_n->ai_addrlen);
    if (status_n == -1) {
        cerr << "Error: cannot connect to socket" << endl;
        return;
    } else
        cout << "Successfully connected to " << req.host << " on port " << req.port << endl;

}

void send_header_to_server(Request req, int dest_fd){
    char reqbuff[BUFF_SIZE];
    memset(reqbuff, 0, BUFF_SIZE);
    //memcpy(reqbuff, req.ori_req.c_str(), req.ori_req.length());
    size_t end_line = req.ori_req.find("\r\n\r\n");
    string header = req.ori_req.substr(0, end_line + 4);
    strcpy(reqbuff, header.c_str());
    if(send(dest_fd, reqbuff, header.length(), 0) == -1){
        cout << "cannot send request header to server" << endl;
        return;
    }
    else{
        cout << "Success send request header to server" << endl;
        //cout << reqbuff << endl;
    }
}

void send_header_to_client(Response response_header, int client_connection_fd){
    char respbuff[BUFF_SIZE];
    memset(respbuff, 0, BUFF_SIZE);
    //memcpy(respbuff, req.ori_req.c_str(), req.ori_req.length());
    size_t end_line = response_header.ori_response.find("\r\n\r\n");
    string header = response_header.ori_response.substr(0, end_line + 4);
    strcpy(respbuff, header.c_str());
    if(send(client_connection_fd, respbuff, header.length(), 0) == -1){
        cout << "cannot send response header to client" << endl;
        return;
    }
    else{
        cout << "Success send response header to client" << endl;
        cout << respbuff << endl;
    }
}


void handle_post(Request req, int ID, int client_connection_fd) {
    int status_n;
    int dest_fd;
    struct addrinfo host_info_n;
    struct addrinfo *host_info_list_n;
    const char *hostname_n = req.host.c_str();
    const char *port_n = "80";
    memset(&host_info_n, 0, sizeof(host_info_n));
    host_info_n.ai_family = AF_UNSPEC;
    host_info_n.ai_socktype = SOCK_STREAM;
    status_n = getaddrinfo(hostname_n, port_n, &host_info_n, &host_info_list_n);
    if (status_n != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        return;
    }
    dest_fd = socket(host_info_list_n->ai_family,
                     host_info_list_n->ai_socktype,
                     host_info_list_n->ai_protocol);

    if (dest_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        return;
    }

    status_n = connect(dest_fd, host_info_list_n->ai_addr, host_info_list_n->ai_addrlen);
    if (status_n == -1) {
        cerr << "Error: cannot connect to socket" << endl;
        return;
    } else
        cout << "Successfully connected to " << req.host << " on port " << req.port << endl;

    //parse header from req
    char reqbuff1[BUFF_SIZE];
    char reqbuff2[BUFF_SIZE];
    memset(reqbuff1, 0, BUFF_SIZE);
    memset(reqbuff2, 0, BUFF_SIZE);
    strcpy(reqbuff1, req.ori_req.c_str());
    ssize_t send_to_dest1 = send(dest_fd, reqbuff1, req.ori_req.length(), 0); // proxy forward request to server before\r\n\r\n(POST header)
    if(send_to_dest1 < 0){
        cout << "len < 0" << endl;
        return;
    }
    char buff1[BUFF_SIZE];
    ssize_t recv_len1 = recv(dest_fd, buff1, BUFF_SIZE, MSG_WAITALL); // proxy receive info from server
    if (recv_len1 < 0) {
        cout << "len < 0" << endl;
        return;
    }
    if (recv_len1 == 0) {
        close(dest_fd);
        return;
    }
    cout << "recv length is " << recv_len1<< " from server:" << endl;
    cout << buff1 << endl;
    ssize_t send_to_client = send(client_connection_fd, buff1, BUFF_SIZE, 0); //proxy returns request to client
    if (send_to_client < 0) {
        cout << "len < 0" << endl;
    }
    close(dest_fd);
    return;
}

void handle_get(Request req, int ID, int client_connection_fd) {
    int status_n;
    int dest_fd;
    struct addrinfo host_info_n;
    struct addrinfo *host_info_list_n;
    const char *hostname_n = req.host.c_str();
    const char *port_n = to_string(req.port).c_str();
    memset(&host_info_n, 0, sizeof(host_info_n));
    host_info_n.ai_family = AF_UNSPEC;
    host_info_n.ai_socktype = SOCK_STREAM;
    status_n = getaddrinfo(hostname_n, port_n, &host_info_n, &host_info_list_n);
    if (status_n != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        return;
    }
    dest_fd = socket(host_info_list_n->ai_family,
                     host_info_list_n->ai_socktype,
                     host_info_list_n->ai_protocol);

    if (dest_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        return;
    }

    status_n = connect(dest_fd, host_info_list_n->ai_addr, host_info_list_n->ai_addrlen);
    if (status_n == -1) {
        cerr << "Error: cannot connect to socket" << endl;
        return;
    } else
        cout << "Successfully connected to " << req.host << " on port " << req.port << endl;

    char reqbuff[BUFF_SIZE];
    memset(reqbuff, 0, BUFF_SIZE);
    size_t end_line = req.ori_req.find("\r\n\r\n");
    string header = req.ori_req.substr(0, end_line + 4);
    strcpy(reqbuff, header.c_str());
    //  cout << "proxy send to server" << endl;
    // cout << reqbuff << endl;

    ssize_t send_to_dest = send(dest_fd, reqbuff, header.length(), 0); // proxy forward request to server before\r\n\r\n
    if(send_to_dest < 0){
        cout << "len < 0" << endl;
        return;
    }

    string s2 = to_string(ID) + ": not in cache";
    add_log(s2);
/*
    int fdmax;
    fd_set readfds;
    char buff[BUFF_SIZE];
    while(1) {
        FD_ZERO(&readfds);
        FD_SET(dest_fd, &readfds);
        FD_SET(client_connection_fd, &readfds);
        fdmax = max(dest_fd, client_connection_fd);
        if (select(fdmax + 1, &readfds, NULL, NULL, NULL) == -1) {
            //print_error(ID,-300);
            cout << "error select" << endl;
            break;
        }

        if (FD_ISSET(client_connection_fd, &readfds)) {
            memset(buff, 0, BUFF_SIZE);
            ssize_t recv_len = recv(client_connection_fd, buff, BUFF_SIZE, MSG_WAITALL);
            if (recv_len < 0) {
                //print_error(ID,-100);
                break;
            }
            if (recv_len == 0) {
                close(dest_fd);
                return;
            }
            //cout << "recv length is " << recv_len<< " from client:" << endl;
            //cout << buff << endl;
            ssize_t send_to_dest = send(dest_fd, buff, recv_len, 0);
            if (send_to_dest < 0) {
                //print_error(ID,-200);
                break;
            }
        } else if (FD_ISSET(dest_fd, &readfds)) {
            memset(buff, 0, BUFF_SIZE);
            ssize_t recv_len = recv(dest_fd, buff, BUFF_SIZE, MSG_WAITALL);
            if (recv_len < 0) {
                //print_error(ID,-100);
                break;
            }
            if (recv_len == 0) {
                close(dest_fd);
                break;
            }
            //cout << "recv length is " << recv_len << " from destination:" << endl;
            //cout << buff << endl;



            ssize_t send_to_client = send(client_connection_fd, buff, recv_len, 0);
            if (send_to_client < 0) {
                //print_error(ID,-200);
                break;
            }
        }
    }
    */
    string s3 = to_string(ID) + ": Requesting \"" + req.request_line + "\" from " + req.host;
    add_log(s3);

    char buff1[BUFF_SIZE];
    ssize_t recv_len1 = recv(dest_fd, buff1, BUFF_SIZE, MSG_WAITALL); // proxy receive info from server
    if (recv_len1 < 0) {
        cout << "len < 0" << endl;
        return;
    }
    if (recv_len1 == 0) {
        close(dest_fd);
        return;
    }

    string tmp(buff1);
    size_t f1 = tmp.find("\r\n\r\n");
    string resp_header = tmp.substr(0, f1 + 4);

    size_t f2 = resp_header.find("\r\n");
    string response_line = resp_header.substr(0, f2);
    string s4 = to_string(ID) + ": Received \"" + response_line + "\" from " + req.host;
    add_log(s4);

    ssize_t send_to_client = send(client_connection_fd, buff1, BUFF_SIZE, 0); //proxy returns request to client
    if (send_to_client < 0) {
        cout << "len < 0" << endl;
    }

    string s5 = to_string(ID) + ": Responding \"" + response_line + "\"";
    add_log(s5);
    close(dest_fd);
}


void handle_connect(Request req, int ID, int client_connection_fd) {

    int dest_fd;
    connect_to_server(req, dest_fd);

    std::string ok("HTTP/1.1 200 Connection Established\r\n\r\n");
    ssize_t ok_ack = send(client_connection_fd, ok.c_str(),strlen(ok.c_str()),0);
    if(ok_ack < 0){
        //print_error(ID,-200);
        cout << "ID: " << ID << "cannot send 200 ok back to client" << endl;
        close(dest_fd);
        return;
    }

    int fdmax;
    fd_set readfds;
    char buff[BUFF_SIZE];
    while(1){
        FD_ZERO(&readfds);
        FD_SET(dest_fd,&readfds);
        FD_SET(client_connection_fd,&readfds);
        fdmax = max(dest_fd, client_connection_fd);
        if(select(fdmax + 1, &readfds, NULL, NULL, NULL) == -1){
            //print_error(ID,-300);
            cout << "error select" << endl;
            break;
        }
        memset(buff, 0, BUFF_SIZE);
        if(FD_ISSET(client_connection_fd, &readfds)){
            ssize_t recv_len = recv(client_connection_fd, buff, BUFF_SIZE, 0);
            if(recv_len < 0){
                //print_error(ID,-100);
                break;
            }
            if(recv_len == 0){
                close(dest_fd);
                return;
            }
            //cout << "recv length is " << recv_len<< " from client:" << endl;
            //cout << buff << endl;
            ssize_t send_to_dest = send(dest_fd, buff, recv_len, 0);
            if(send_to_dest < 0){
                //print_error(ID,-200);
                break;
            }
        }
        else if(FD_ISSET(dest_fd, &readfds)){
            ssize_t recv_len = recv(dest_fd, buff, BUFF_SIZE, 0);
            if(recv_len < 0){
                //print_error(ID,-100);
                break;
            }
            if(recv_len == 0){
                close(dest_fd);
                break;
            }
            //cout << "recv length is " << recv_len << " from destination:" << endl;
            //cout << buff << endl;
            ssize_t send_to_client = send(client_connection_fd, buff, recv_len, 0);
            if(send_to_client < 0){
                //print_error(ID,-200);
                break;
            }
        }
    }
    close(dest_fd);
    return;
}




void multi_thread(int ID, int client_connection_fd, string ip){
    time_t tmp_time = time(NULL);  //get current time
    char * curr_time = asctime(gmtime(&tmp_time));
    string stime(curr_time);

    char reqbuff[BUFF_SIZE];
    ssize_t recv_len = recv(client_connection_fd, reqbuff, BUFF_SIZE, 0);
    reqbuff[recv_len] = 0;
    if(recv_len == -1){
        cout << "cannot recv request from client" << endl;
        return;
    }
    else if(recv_len == 0){
        cout << "recv nothing from client" << endl;
        return;
    }
    cout << "current time is: " << curr_time << endl;
    cout << reqbuff << endl;

    Request req(reqbuff);
    cout << "req.method: " << req.method << endl;
    cout << "req.host: " << req.host << endl;
    cout << "req.port: " << req.port << endl;
    cout << "req.request_len: " << req.request_line << endl;
    cout << endl;

    string s1 = to_string(ID) + ": \"" + req.request_line + "\" from " + ip + " @ " + stime;
    add_log(s1);
    if(req.method == "CONNECT"){
        handle_connect(req, ID, client_connection_fd);
    }
    else if(req.method == "GET"){
        handle_get(req, ID, client_connection_fd);
    }
    else if(req.method == "POST"){
        //handle_post(req, ID, client_connection_fd);
    }


}

int main(int argc, char *argv[]) {


//************ server part: accept connection from browser ***********//
    int ID = 0;
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname = NULL;
    const char *port = "12345";

    //initialize as server to browser waiting to accept
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        cerr << "Error: cannot get address info for host" << endl;
        return -1;
    }

    socket_fd = socket(host_info_list->ai_family,
                       host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
        cerr << "Error: cannot create socket" << endl;
        return -1;
    }

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        cerr << "Error: cannot bind socket" << endl;
        return -1;
    }

    status = listen(socket_fd, 100);
    if (status == -1) {
        cerr << "Error: cannot listen on socket" << endl;
        return -1;
    }

    while (1) {
        ID++;
        //********** server part: wait to accept browser request ********//
        cout << "================== Waiting for connection on port " << port << " ==========================" << endl;
        cout << "ID = " << ID << endl;
        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        int client_connection_fd;
        client_connection_fd = accept(socket_fd, (struct sockaddr *) &socket_addr, &socket_addr_len);
        if (client_connection_fd == -1) {
            cerr << "Error: cannot accept connection on socket" << endl;
            return -1;
        }

        string ip(inet_ntoa(((struct sockaddr_in *) &socket_addr)->sin_addr));
        //cout << "Browser ip: " << ip << endl;

        thread(multi_thread, ID, client_connection_fd, ip).detach();

    }

    freeaddrinfo(host_info_list);
    close(socket_fd);

    return 0;
}

