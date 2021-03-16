#include "itwc_handler.h"
#include "mime_types.h"
#include "make_response.h"
#include "log_handler.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <vector>
#define GetCurrentDir getcwd



// Constructor
itwc_handler::itwc_handler(std::string location_prefix, NginxConfig &config)
             :base_request_handler(location_prefix, config),
              set_dirs_() {}


itwc_handler* itwc_handler::Init(std::string location_prefix, NginxConfig &config) 
{ return new itwc_handler(location_prefix, config); }


response itwc_handler::do_handle(request &req) {
    base_request_handler::inc_number_of_handlers("ITWC");

    // clean up (remove hexcode) the url
    response resp;
    log_data log;
    log.request_handler = "ITWC";
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


    // special case handling
    if (request_path == "/itwc/") {
        request_path += "html/front.html";
    }
    else if (request_path.size() >= 13 && request_path.substr(0, 13) == "/itwc/winner/") {
        add_winner(request_path);
        resp.status = response::ok;
        return resp;
    }

    req.path = root_path_ + real_url(request_path);


    handle_log(2, "ITWC handler accessing front.html");
    char DIR[512];
    GetCurrentDir(DIR, 512);
    std::string current_working_dir(DIR);
    std::ifstream input(req.path.c_str(), std::ios::in | std::ios::binary);
    if (!input) {
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


bool itwc_handler::is_dir_exist(std::string &set_name)
{ return set_dirs_.find(set_name) != set_dirs_.end(); }

std::string itwc_handler::get_dir(std::string &set_name)
{ return set_dirs_[set_name]; }

bool itwc_handler::set_dir(std::string &set_name, std::string &dir_name) {
    // check if the set exists
    if (is_dir_exist(set_name))
        return false;

    set_dirs_[set_name] = dir_name;
    return true;
}

void itwc_handler::add_winner(std::string &winner) {
    // parse the winner information
    std::string winner_info = winner.substr(13);
    size_t index = 0;
    while (index < winner_info.size() && winner_info[index] != '/')
        ++index;
    std::string set_name = winner_info.substr(0, index);
    std::string file_name = winner_info.substr(index+1);

    // initialize variables for opening existing file
    std::map<std::string, int> score_map;
    std::string file_path = root_path_;
    file_path += "/score_map/";
    file_path += set_name;
    file_path += ".txt";

    // open the file to retrieve data
    // if the file does not exist, we can still continue
    std::ifstream score_text(file_path.c_str());
    if (score_text.is_open()) {
        // successfully opened, now pull previous data from it
        std::string line;
        while (getline(score_text, line)) {
            std::string curr_file = line.substr(0, line.find("@"));
            int count = atoi(line.substr(line.find("@") + 1).c_str());
            score_map[curr_file] = count;
        }
        score_text.close();
    }
    score_map[file_name]++;

    // sort the list and write to file
    std::vector<std::pair<int, std::string>> v;
    for (auto it = score_map.begin(); it != score_map.end(); ++it) {
        v.push_back(std::pair<int, std::string> (it->second, it->first));
    }
    sort(v.begin(), v.end(), std::greater <>());

    std::string result = "";
    for (int i = 0; i < v.size(); ++i) {
        result.append(v[i].second);
        result.append("@");
        result.append(std::to_string(v[i].first));
        result.append("\n");
    }
    result.pop_back();
    std::ofstream write_text(file_path.c_str());
    if (write_text.is_open()) {
        write_text << result;
        write_text.close();
    }

    return;
}
