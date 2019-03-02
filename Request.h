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

class Request { // CONNECT
public:
    string ori_req; //original request
    string method;
    string host;
    int port;
    string URI;

    explicit Request(string request):ori_req(request), port(0){
        method = get_method();
	if (method == "CONNECT") {
	  host = get_host();
	  port = get_port();
	}
	if (method == "GET") {
	  host = get_host();
          port = get_port();
	  URI = get_URI();
	}
    }
    
    string get_method();
    string get_host();
    int get_port();
    string get_URI();
};




#endif //HTTP_PROXY_REQUEST_H
