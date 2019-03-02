//
// Created by Guanwen Wang on 2019-02-27.
//

#include "Response.h"

size_t Response::get_content_length() {
    int con_len;
    size_t cl = ori_response.find("Content-Length");
    string tmp = ori_response.substr(cl);
    size_t end_line = tmp.find("\r\n");
    string len_num = tmp.substr(16, end_line - 16);
    //cout << "len = " << len_num << " **********" << endl;
    con_len = stoi(len_num, NULL, 0);
    //cout << "con_len = " << con_len << endl;
    return con_len;
}