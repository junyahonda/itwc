#include "make_response.h"
#include "log_handler.h"

// default status message for respond message
namespace status_message {
    const std::string ok =
        "HTTP/1.0 200 OK\r\n";

    const std::string created =
        "HTTP/1.0 201 Created\r\n";

    const std::string accepted =
        "HTTP/1.0 202 Accepted\r\n";

    const std::string no_content =
        "HTTP/1.0 204 No Content\r\n";

    const std::string multiple_choices =
        "HTTP/1.0 300 Multiple Choices\r\n";

    const std::string moved_permanently =
        "HTTP/1.0 301 Moved Permanently\r\n";

    const std::string moved_temporarily =
        "HTTP/1.0 302 Moved Temporarily\r\n";

    const std::string not_modified =
        "HTTP/1.0 304 Not Modified\r\n";

    const std::string bad_request =
        "HTTP/1.0 400 Bad Request\r\n";

    const std::string unauthorized =
        "HTTP/1.0 401 Unauthorized\r\n";

    const std::string forbidden =
        "HTTP/1.0 403 Forbidden\r\n";

    const std::string not_found =
        "HTTP/1.0 404 Not Found\r\n";

    const std::string internal_server_error =
        "HTTP/1.0 500 Internal Server Error\r\n";

    const std::string not_implemented =
        "HTTP/1.0 501 Not Implemented\r\n";

    const std::string bad_gateway =
        "HTTP/1.0 502 Bad Gateway\r\n";

    const std::string service_unavailable =
        "HTTP/1.0 503 Service Unavailable\r\n";

    // Convert a status to corresponding message
    boost::asio::const_buffer status_to_buffer(response::status_type status) {
        switch (status) {
        case response::ok:
            return boost::asio::buffer(ok);

        case response::created:
            return boost::asio::buffer(created);

        case response::accepted:
            return boost::asio::buffer(accepted);

        case response::no_content:
            return boost::asio::buffer(no_content);

        case response::multiple_choices:
            return boost::asio::buffer(multiple_choices);

        case response::moved_permanently:
            return boost::asio::buffer(moved_permanently);

        case response::moved_temporarily:
            return boost::asio::buffer(moved_temporarily);

        case response::not_modified:
            return boost::asio::buffer(not_modified);

        case response::bad_request:
            return boost::asio::buffer(bad_request);

        case response::unauthorized:
            return boost::asio::buffer(unauthorized);

        case response::forbidden:
            return boost::asio::buffer(forbidden);

        case response::not_found:
            return boost::asio::buffer(not_found);

        case response::internal_server_error:
            return boost::asio::buffer(internal_server_error);

        case response::not_implemented:
            return boost::asio::buffer(not_implemented);

        case response::bad_gateway:
            return boost::asio::buffer(bad_gateway);

        case response::service_unavailable:
            return boost::asio::buffer(service_unavailable);

        default:
            return boost::asio::buffer(internal_server_error);
        }
    }

} // namespace status_message


std::vector<boost::asio::const_buffer> response::to_buffers() {
    // create a vector of buffer to return
    
    std::vector<boost::asio::const_buffer> result_buffers;

    // insert status message in front


    result_buffers.push_back(status_message::status_to_buffer(status));
    for (auto itr = header_map.begin(); itr != header_map.end(); itr++){
        result_buffers.push_back(boost::asio::buffer(itr->first));
        result_buffers.push_back(boost::asio::buffer(name_value_separator));
        result_buffers.push_back(boost::asio::buffer(itr->second));
        result_buffers.push_back(boost::asio::buffer(crlf)); 
    }

    result_buffers.push_back(boost::asio::buffer(crlf));
    result_buffers.push_back(boost::asio::buffer(data));


    return result_buffers;
}


// default html message for each status
namespace http_response {
    const char ok[] = 
        "";

    const char created[] =
        "<html>"
        "<head><title>Created</title></head>"
        "<body><h1>201 Created</h1></body>"
        "</html>";

    const char accepted[] =
        "<html>"
        "<head><title>Accepted</title></head>"
        "<body><h1>202 Accepted</h1></body>"
        "</html>";

    const char no_content[] =
        "<html>"
        "<head><title>No Content</title></head>"
        "<body><h1>204 Content</h1></body>"
        "</html>";

    const char multiple_choices[] =
        "<html>"
        "<head><title>Multiple Choices</title></head>"
        "<body><h1>300 Multiple Choices</h1></body>"
        "</html>";

    const char moved_permanently[] =
        "<html>"
        "<head><title>Moved Permanently</title></head>"
        "<body><h1>301 Moved Permanently</h1></body>"
        "</html>";

    const char moved_temporarily[] =
        "<html>"
        "<head><title>Moved Temporarily</title></head>"
        "<body><h1>302 Moved Temporarily</h1></body>"
        "</html>";

    const char not_modified[] =
        "<html>"
        "<head><title>Not Modified</title></head>"
        "<body><h1>304 Not Modified</h1></body>"
        "</html>";

    const char bad_request[] =
        "<html>"
        "<head><title>Bad Request</title></head>"
        "<body><h1>400 Bad Request</h1></body>"
        "</html>";

    const char unauthorized[] =
        "<html>"
        "<head><title>Unauthorized</title></head>"
        "<body><h1>401 Unauthorized</h1></body>"
        "</html>";

    const char forbidden[] =
        "<html>"
        "<head><title>Forbidden</title></head>"
        "<body><h1>403 Forbidden</h1></body>"
        "</html>";

    const char not_found[] =
        "<html>"
        "<head><title>Not Found</title></head>"
        "<body><h1>404 Not Found</h1></body>"
        "</html>";

    const char internal_server_error[] =
        "<html>"
        "<head><title>Internal Server Error</title></head>"
        "<body><h1>500 Internal Server Error</h1></body>"
        "</html>";

    const char not_implemented[] =
        "<html>"
        "<head><title>Not Implemented</title></head>"
        "<body><h1>501 Not Implemented</h1></body>"
        "</html>";
    const char bad_gateway[] =
        "<html>"
        "<head><title>Bad Gateway</title></head>"
        "<body><h1>502 Bad Gateway</h1></body>"
        "</html>";

    const char service_unavailable[] =
        "<html>"
        "<head><title>Service Unavailable</title></head>"
        "<body><h1>503 Service Unavailable</h1></body>"
        "</html>";

    // Convert a status to corresponding http string
    std::string status_to_http(response::status_type status){
        switch (status) {
        case response::ok:
            return ok;

        case response::created:
            return created;

        case response::accepted:
            return accepted;

        case response::no_content:
            return no_content;

        case response::multiple_choices:
            return multiple_choices;

        case response::moved_permanently:
            return moved_permanently;

        case response::moved_temporarily:
            return moved_temporarily;

        case response::not_modified:
            return not_modified;

        case response::bad_request:
            return bad_request;

        case response::unauthorized:
            return unauthorized;

        case response::forbidden:
            return forbidden;

        case response::not_found:
            return not_found;

        case response::internal_server_error:
            return internal_server_error;

        case response::not_implemented:
            return not_implemented;

        case response::bad_gateway:
            return bad_gateway;

        case response::service_unavailable:
            return service_unavailable;

        default:
            return internal_server_error;
        }
    }
} // namespace http_response   

// Create response message to return
response response::status_to_response(response::status_type status) {
    response return_resp;
    return_resp.status = status;
    handle_log(1, "Status code: " + status);
    // std::cout << "status code is: " << status << std::endl;
    base_request_handler::add_to_code_map(std::to_string(status));
    return_resp.data = http_response::status_to_http(status);

    return_resp.header_map["Content-Length"] = std::to_string(return_resp.data.size());
    return_resp.header_map["Content-Type"] = "text/html";
    return return_resp;
}

// Create response message to return as ECHO
response response::status_to_response_echo(response::status_type status, const request &req) {
    response return_resp;
    return_resp.status = status;
    handle_log(1, "Status code: " + status);
    // std::cout << "status code is: " << status << std::endl;
    base_request_handler::add_to_code_map(std::to_string(status));
    return_resp.data = req.raw;

    return_resp.header_map["Content-Length"] = std::to_string(return_resp.data.size());
    return_resp.header_map["Content-Type"] = "text/plain";

    return return_resp;
}

// Create response message to return as STATUS
response response::response_status() {
    response return_resp;

    std::string resp = "<!doctype html>\n";
    resp += "<html> <head> <title> Status Page </title> </head> <body> \n";


    resp += "<p>\n";
    resp += "Total number of handler calls: " + std::to_string(base_request_handler::total_handler()) + "\n";

    resp += "<br/></p><p>\n";

    resp += "\nNumber of calls by handler type: <br/>\n\n";
    resp += "Total number of echo calls: " + std::to_string(base_request_handler::get_number_of_type("echo")) + "<br/>\n";
    resp += "Total number of static calls: " + std::to_string(base_request_handler::get_number_of_type("static")) + "<br/>\n";
    resp += "Total number of status calls: " + std::to_string(base_request_handler::get_number_of_type("status")) + "<br/>\n";
    resp += "Total number of not found calls: " + std::to_string(base_request_handler::get_number_of_type("404")) + "<br/>\n";
    resp += "Total number of reverse proxy calls: " + std::to_string(base_request_handler::get_number_of_type("proxy")) + "<br/>\n";
    resp += "Total number of health calls: " + std::to_string(base_request_handler::get_number_of_type("health")) + "<br/>\n";
    resp += "Total number of ITWC calls: " + std::to_string(base_request_handler::get_number_of_type("ITWC")) + "<br/>\n";

    resp += "<br/></p><p>\n";

    std::map<std::string, int> resp_code_map = base_request_handler::get_response_code_map();
    resp += "\nNumber of calls by response code: <br/>\n\n";
    for( auto const& [key, val] : resp_code_map) {
        resp += "Total number of requests with return code " + key + ": " + std::to_string(val) + "<br/>\n";
    }
    
    resp += "<br/></p><p>\n";

    std::map<std::string, int> url_map = base_request_handler::get_number_of_url();
    resp += "\nNumber of calls by url: <br/>\n\n";
    for( auto const& [key, val] : url_map) {
        resp += "Total number of requests for \"" + key + "\": " + std::to_string(val) + "<br/>\n";
    }

    resp += "<br/></p><p>\n";

    std::map<std::string,std::vector<std::string>> handler_map = base_request_handler::get_all_handlers();
    resp += "\nAvailable Handlers and their URL prefix(es): <br/>\n";
    for( auto const& [key, val] : handler_map) {
        resp += key + " path(s): ";
        for (auto i : val) {
            resp += i + ", ";
        }
        //to remove the last , and whitespace
        resp.pop_back();
        resp.pop_back();

        resp += "<br/>\n";
    }

    resp += "<br/></p>\n";
    resp += "</body></html>";


    return_resp.data = resp;

    return_resp.header_map["Content-Length"] = std::to_string(return_resp.data.size());
    return_resp.header_map["Content-Type"] = "text/html";

    return return_resp;
}
