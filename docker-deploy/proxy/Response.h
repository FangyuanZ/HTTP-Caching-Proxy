//
// Created by Guanwen Wang on 2019-02-27.
//

#ifndef HTTP_PROXY_RESPONSE_H
#define HTTP_PROXY_RESPONSE_H
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <string>
#include <string.h>

using namespace std;

class Response {
public:
    string ori_response;
    size_t content_length;

    explicit Response(string response_header): ori_response(response_header){
        content_length = get_content_length();
    }

    size_t get_content_length();
};


#endif //HTTP_PROXY_RESPONSE_H
