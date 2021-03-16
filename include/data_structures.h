#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>
#include <vector>
#include <map>
#include "config_parser.h"

// header data structure
// i.e. host: 111.222.111.222  =  name + value
//      name = "host"
//      value = "111.222.111.222"
struct header {
    std::string name;
    std::string value;
};

// request data structure
// i.e. GET /static/test.mp3 HTTP/1.1
//      ...
// method   = GET
// uri      = /static/test.mp3
// http_version_major = 1.1
// http_version_minor = 0
// headers = rest inputs go here
// path = path within file system
// raw = original raw request strings
// ip = ip address of the client
struct request {
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::vector<header> headers;
    // keep this previous format(the above vector) to parse the request
    // to follow the common API format, created the unordered map 
    // to store the data
    std::map<std::string, std::string> headers_map;
    std::string path = "";
    std::string raw = "";
    std::string ip = "";
};

// handler config data structure
// i.e. location "/static" StaticHanlder {
//         root "./files";
//      }
// type = handler_config::static
// url = "/static"
// path = "./files"
struct handler_config {
    enum {Echo,
          Static,
          Not_found,
          Status,
          Reverse_proxy,
          Health,
          ITWC} type;
    std::string location_prefix;
    NginxConfig config;
};

struct log_data {
    std::string response_code;
    std::string request_handler;
    std::string ip_address;
};


const char name_value_separator[] = { ':', ' '};
const char crlf[] = { '\r', '\n' };


#endif // DATA_STRUCTURES_H