#include "echo_request_handler.h"
#include "mime_types.h"
#include "make_response.h"
#include "log_handler.h"
#include "data_structures.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#define GetCurrentDir getcwd



// Constructor
echo_request_handler::echo_request_handler(std::string location_prefix, NginxConfig &config)
                     :base_request_handler(location_prefix, config) { }

echo_request_handler* echo_request_handler::Init(std::string location_prefix, NginxConfig &config) {
    return new echo_request_handler(location_prefix, config);
}

response echo_request_handler::do_handle(request &req) {
    base_request_handler::inc_number_of_handlers("echo");
    response resp;

    log_data log;
    log.request_handler = "echo";
    log.ip_address = req.ip;

    // clean up (remove hexcode) the url
    std::string request_path;
    if (!clean_url(req.uri, request_path)) {
        // unable to clean the url which indicates the
        // invalid url 
        base_request_handler::add_to_url_map(request_path);
        resp = response::status_to_response(response::bad_request);
        log.response_code = std::to_string(response::bad_request);
        handle_machine_log(log, 4, "[Message] invalid url");
        return resp;
    }

    // another invalid path (url) cases
    // 1. path cannot be empty
    // 2. path cannot start other than '/'
    // 3. path cannot contain ".."
    if (request_path.empty()   || 
        request_path[0] != '/' || 
        request_path.find("..") != std::string::npos) {
        // bad url detected
        resp = response::status_to_response(response::bad_request);
        
        base_request_handler::add_to_url_map(request_path);
        log.response_code = std::to_string(response::bad_request);
        handle_machine_log(log, 4, "[Message] invalid url - illegal path");
        return resp;
    }

    // if path ends with '/', use index.html in that path
    if (request_path[request_path.size() - 1] == '/') {
        request_path += "index.html";
    }

    // if file extension exists, parse them
    // i.e. /.../static/temp.txt -> txt
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos   = request_path.find_last_of(".");
    std::string file_extension;
    if (last_dot_pos != std::string::npos &&
        last_dot_pos > last_slash_pos) {
        file_extension = request_path.substr(last_dot_pos + 1);
    }

    // create the full path to the file in the system
    // store the path to request object so derived class can access
    // request_path = real_url(req.uri);
    // request_path = root_path_ + real_url(request_path);
    req.path = root_path_ + real_url(request_path);



    // try to open the file into input stream as in both ios in or binary
    // append the root_path_ in front
    handle_log(2, "Trying to access a file as Echo..."); 
     

    char DIR[512];
    GetCurrentDir(DIR, 512);
    std::string current_working_dir(DIR);
    std::ifstream input(req.path.c_str(), std::ios::in | std::ios::binary);
    if (!input) {
        // failed to open the file, must exit with not found message
        resp = response::status_to_response_echo(response::not_found, req);
        base_request_handler::add_to_url_map(request_path);

        log.response_code = std::to_string(response::bad_request);
        handle_machine_log(log, 4, "[Message] invalid url - file does not exist");
        return resp;
    }

    // file opened successfully. echo back the original request message
    resp.status = response::ok;
    resp.data = req.raw;
    resp.header_map["Content-Length"] = std::to_string(resp.data.size());
    resp.header_map["Content-Type"] = "text/plain";
    
    base_request_handler::add_to_code_map("200");
    base_request_handler::add_to_url_map(request_path);

    log.response_code = std::to_string(response::ok);
    handle_machine_log(log, 2, "[Message] Response created successfully");

    return resp;
}