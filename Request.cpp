#include "Request.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ostream>
#include <string>
#include <string.h>


//Get information from header file

string Request::get_method(){ // make sure the request type: GET POST CONNECT
    string _method;
    size_t first_blank = ori_req.find(' ');
    _method = ori_req.substr(0, first_blank);
    return _method;
}

string Request::get_host(){  // get host name
    string _host;
    size_t tmp1 = ori_req.find("Host: ") + 6;
    size_t tmp2 = ori_req.find("\r\n", tmp1);
    string tmpstr = ori_req.substr(tmp1, tmp2 - tmp1);
    size_t last_colon = tmpstr.find_last_of(':');
    if(last_colon == string::npos){
        return tmpstr;
    }
    _host = tmpstr.substr(0, last_colon);
    return _host;
}

int Request::get_port(){ // get port number
    int _port = 80;
    size_t tmp1 = ori_req.find("Host: ") + 6;
    size_t tmp2 = ori_req.find("\r\n", tmp1);
    string tmpstr = ori_req.substr(tmp1, tmp2 - tmp1);
    size_t last_colon = tmpstr.find_last_of(':');
    if(last_colon != string::npos){
        _port = stoi(tmpstr.substr(last_colon + 1, tmpstr.length() - last_colon - 1));
    }
    return _port;
}

string Request::get_URI() {
  string _URI;
  size_t first_blank = ori_req.find(' ');
  string temp = ori_req.substr(first_blank + 1);
  size_t second_blank = temp.find(' ');
  _URI = temp.substr(0, second_blank);
  return _URI;
}
