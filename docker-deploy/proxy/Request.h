//
// Created by Guanwen Wang on 2019-02-23.
//

#ifndef HTTP_PROXY_REQUEST_H
#define HTTP_PROXY_REQUEST_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <string>
#include <string.h>

using namespace std;

class Request {
public:
    string ori_req; //original request
    string method;
    string host;
    int port;
    string request_line;

    explicit Request(string request):ori_req(request), port(0){
        method = get_method();
        host = get_host();
        port = get_port();
        request_line = get_request_line();

    }

    string get_method();
    string get_host();
    int get_port();
    string get_request_line();


};



#endif //HTTP_PROXY_REQUEST_H
