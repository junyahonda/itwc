#ifndef HANDLE_DISPATCH_H
#define HANDLE_DISPATCH_H
#include <map>
#include "base_request_handler.h"
struct request;


class handle_dispatch {
public:
	// handle_dispatch's constructor
	// Argument : map
	// Output  : -
	// it is just simply initializing the variables and objects
	handle_dispatch(std::map<std::string, base_request_handler*> &mp_handler);


	// match_handler
	// Argument : request passing by reference
	// Output   : reponse
	// call the find_match to match with the longest url 
	// then generate response message by creating corresponding request handlers and 
	// call do_hanlde function
	response match_handler(request &req);

private:
	// find_match
	// Argument : (string) url
	// Output   : base_request_handler*
	// it finds a longest match from url.
	// and returns the proper request type, which is the sub-request handlers
	base_request_handler* find_match(std::string url);
	std::map<std::string, base_request_handler*> &handlers_;
};

#endif