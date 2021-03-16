#include "handle_dispatch.h"
#include "base_request_handler.h"
#include "data_structures.h"
#include "make_response.h"
#include "log_handler.h"
#include "status_request_handler.h"
#include "config_parser.h"

handle_dispatch::handle_dispatch(std::map<std::string, base_request_handler*> &mp_handler):handlers_(mp_handler) {}

// use the request's longest url to match the location 
// and generate response with teh request
response handle_dispatch::match_handler(request &req) {
	NginxConfig config;
	std::string url = req.uri;
	base_request_handler* handler = find_match(url);
	response new_resp;
	new_resp = handler->do_handle(req);
	return new_resp; 
}


// find the longest match of url 
base_request_handler* handle_dispatch::find_match(std::string url) {
	int longest = 0;
	for (int i = 0; i < url.size(); i++){
		if (url[i] == '/') {
			longest = i;
		}
	}
	std::string status_catch = url.substr(0,7);
	if(status_catch == "/status"){
		if(handlers_.find(status_catch) != handlers_.end())
			return handlers_[status_catch];
	}
    std::cout << "HANDLERS:\n";
    for (auto it = handlers_.begin(); it != handlers_.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
	while (longest >= 0) {
		if (longest == 0) {
			return handlers_["/"];
		}
		else if (url[longest] != '/') {
			longest--;
		}
		else if (handlers_.find(url.substr(0, longest)) != handlers_.end()) {
			return handlers_[url.substr(0, longest)];
		}
		else {
			longest--;
		}
	}
}
