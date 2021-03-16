#include "status_request_handler.h"
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
status_request_handler::status_request_handler(std::string location_prefix, NginxConfig &config)
                       :base_request_handler(location_prefix, config) {}

status_request_handler* status_request_handler::Init(std::string location_prefix, NginxConfig &config) {
    return new status_request_handler(location_prefix, config);
}

response status_request_handler::do_handle(request &req) {

    log_data log;
    log.request_handler = "status";
    log.ip_address = req.ip;
    log.response_code = std::to_string(response::ok);
    handle_machine_log(log, 2, "[Message] Response created successfully.");

    // // clean up (remove hexcode) the url
    response resp;
    base_request_handler::inc_number_of_handlers("status");
    base_request_handler::add_to_code_map("200");

    // need to make this in make_response
    resp = response::response_status();

    resp.status = response::ok;
    return resp;
}