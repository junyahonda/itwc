#include <string>
#include "data_structures.h"
#include "make_response.h"
#include "log_handler.h"
#include "not_found_handler.h"


// Constructor
not_found_handler::not_found_handler(std::string location_prefix, NginxConfig &config)
                       :base_request_handler(location_prefix, config) {}

not_found_handler* not_found_handler::Init(std::string location_prefix, NginxConfig &config) {
	return new not_found_handler(location_prefix, config);
}


response not_found_handler::do_handle(request &req){
	base_request_handler::inc_number_of_handlers("404");
	response resp;
	resp = response::status_to_response(response::not_found);

    log_data log;
    log.response_code = std::to_string(response::not_found);
    log.request_handler = "not found";
    log.ip_address = req.ip;
    handle_machine_log(log, 2, "[Message] Response created successfully");

    return resp;
}