#include "static_request_handler.h"
#include "mime_types.h"
#include "make_response.h"
#include "log_handler.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#define GetCurrentDir getcwd



// Constructor
static_request_handler::static_request_handler(std::string location_prefix, NginxConfig &config)
                       :base_request_handler(location_prefix, config) {}

static_request_handler* static_request_handler::Init(std::string location_prefix, NginxConfig &config) {
    return new static_request_handler(location_prefix, config);
}

response static_request_handler::do_handle(request &req) {
    base_request_handler::inc_number_of_handlers("static");
    
    // clean up (remove hexcode) the url
    response resp;
    log_data log;
    log.request_handler = "static";
    log.ip_address = req.ip;
    std::string request_path;
    
    if (!clean_url(req.uri, request_path)) {
        // unable to clean the url which indicates the
        // invalid url 
        resp = response::status_to_response(response::bad_request);
        log.response_code = std::to_string(response::bad_request);
        base_request_handler::add_to_url_map(request_path);
        handle_machine_log(log, 4, "[Message] invalid url received - unable to clean url");
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
        handle_machine_log(log, 4, "[Message] invalid url received - illegal path");
        return resp;
    }

    // if path ends with '/', use index.html in that path
    if (request_path[request_path.size() - 1] == '/') {
        request_path += "index.html";
    }

    // create the full path to the file in the system
    // store the path to request object so derived class can access
    req.path = root_path_ + real_url(request_path);


    handle_log(2, "Trying to access a file as Static...");
    //std::cerr << "Try to access a file as Static: " << req.path << std::endl;
    char DIR[512];
    GetCurrentDir(DIR, 512);
    std::string current_working_dir(DIR);
    //std::cerr << "Server's WD is: " << current_working_dir << std::endl;
    std::ifstream input(req.path.c_str(), std::ios::in | std::ios::binary);
    if (!input) {
        // failed to open the file, must exit with not found message
        resp = response::status_to_response(response::not_found);
        base_request_handler::add_to_url_map(request_path);
        log.response_code = std::to_string(response::not_found);
        handle_machine_log(log, 4, "[Message] invalid url received - file does not exist");
        return resp;
    }
    handle_log(2, "Access Successful.");

    // file opened successfully. transfer each byte to buffer
    // within the response message
    resp.status = response::ok;
    char buffer[512];
    
    // while input is read, continue to add it to content
    while (input.read(buffer, sizeof(buffer)).gcount() > 0) {
        resp.data.append(buffer, input.gcount());   
    }

    // if file extension exists, parse them
    // i.e. /.../static/temp.txt -> txt
    std::size_t last_slash_pos = req.path.find_last_of("/");
    std::size_t last_dot_pos   = req.path.find_last_of(".");
    std::string file_extension;
    if (last_dot_pos != std::string::npos &&
        last_dot_pos > last_slash_pos) {
        file_extension = req.path.substr(last_dot_pos + 1);
    }
    resp.header_map["Content-Length"] = std::to_string(resp.data.size());
    resp.header_map["Content-Type"] = get_ext_type(file_extension);
    
    base_request_handler::add_to_code_map("200");
    base_request_handler::add_to_url_map(request_path);

    log.response_code = std::to_string(response::ok);
    handle_machine_log(log, 2, "[Message] Response created successfully");
    return resp;
}