#include "make_response.h"
#include "health_request_handler.h"
#include "log_handler.h"


health_request_handler::health_request_handler(std::string location_prefix, NginxConfig &config)
                       :base_request_handler(location_prefix, config) {}



health_request_handler* health_request_handler::Init(std::string location_prefix, NginxConfig &config)
{ return new health_request_handler(location_prefix, config); }



response health_request_handler::do_handle(request &req) {
    base_request_handler::inc_number_of_handlers("health");
    base_request_handler::add_to_code_map("200");

    log_data log;
    log.request_handler = "health";
    log.ip_address = req.ip;
    log.response_code = std::to_string(response::ok);
    handle_machine_log(log, 2, "[Message] Response created successfully");

    response resp;
    resp.status = response::ok;
    resp.data   = "OK";
    resp.header_map["Content-Type"]   = "text/plain";
    resp.header_map["Content-Length"] = std::to_string(resp.data.size());
    return resp;
}