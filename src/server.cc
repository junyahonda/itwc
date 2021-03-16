#include "data_structures.h"
#include "echo_request_handler.h"
#include "health_request_handler.h"
#include "log_handler.h"
#include "not_found_handler.h"
#include "reverse_proxy_handler.h"
#include "server.h"
#include "static_request_handler.h"
#include "status_request_handler.h"
#include "itwc_handler.h"
#include <signal.h>
#include <utility>
#include <boost/thread/thread.hpp>

server::server(const std::string &addr,
               const std::string &port,
               /*std::map<std::string, std::string> &paths*/
               std::vector<handler_config> list_handlers) /* accepts vector of handler_config objects, which contains more info */
       : io_service_(),
         signals_(io_service_),
         acceptor_(io_service_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::stoi(port))),
         session_manager_(),
         socket_(io_service_),
         request_handler_(std::map<std::string, base_request_handler*>()),
         thread_pool_size_(8)
{
    // register signal handlers to tell when to quit/exit
    signals_.add(SIGINT);
    signals_.add(SIGTERM);


    #if defined(SIGQUIT)
        signals_.add(SIGQUIT);
    #endif // defined(SIGQUIT)

    // depending on the type of the request, add handlers for each path
    // base_request_handler *new_handle;
    init_handlers(list_handlers);

    wait_stop();

    acceptor_.listen();

    start_accept(); 

}

server::~server() {
    handle_log(2, "Closing server...");
    // since the request handlers are instantiated when creating handler,
    // delete the created handlers when closing the server.
    for (auto it = request_handler_.begin(); it != request_handler_.end(); it++)
        delete (it->second);
    handle_log(2, "Server successfully closed.");
}

void server::run() {
    // The io_service::run() call will block until all asynchronous operations
    // have finished. While the server is running, there is always at least one
    // asynchronous operation outstanding: the asynchronous accept call waiting
    // for new incoming connections.
    // io_service_.run();
    std::vector<boost::shared_ptr<boost::thread> > threads;
    for (std::size_t i = 0; i < thread_pool_size_; ++i) {
        boost::shared_ptr<boost::thread> thread(new boost::thread(
            boost::bind(&boost::asio::io_service::run, &io_service_)));
        threads.push_back(thread);
    }

    for (std::size_t i = 0; i < threads.size(); ++i) {
        threads[i]->join();
    }
}


void server::start_accept() {
    acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec) {
            // Check whether the server was stopped by a signal before this
            // completion handler had a chance to run.
            if (!acceptor_.is_open()) {
                return;
            }
            
            // if read without error, start the session manager with new session
            if (!ec) {
              session_manager_.start(std::make_shared<session>(
                  std::move(socket_), session_manager_, request_handler_));
            }
        
            start_accept();
        });
}


void server::wait_stop() {
    signals_.async_wait(
        [this](boost::system::error_code /*ec*/, int /*signo*/) {
            // The server is stopped by cancelling all outstanding asynchronous
            // operations. Once all operations have finished the io_service::run()
            // call will exit.
            acceptor_.close();
            session_manager_.stop_all();
            exit(0);
        });
}

base_request_handler* server::createHandler(int type, std::string location_prefix, NginxConfig &config) {
    if (type == handler_config::Echo) {
        return echo_request_handler::Init(location_prefix, config);
    }
    else if (type == handler_config::Static) {
        return static_request_handler::Init(location_prefix, config);
    }
    // for status handler 
    else if (type == handler_config::Status) {
        return status_request_handler::Init(location_prefix, config);
    }
    else if (type == handler_config::Reverse_proxy) {
        return reverse_proxy_handler::Init(location_prefix, config);
    }
    else if (type == handler_config::Health) {
        return health_request_handler::Init(location_prefix, config);
    }
    else if (type == handler_config::ITWC) {
        return itwc_handler::Init(location_prefix, config);
    }
    else {
        return not_found_handler::Init(location_prefix, config);
    }
}

void server::init_handlers(std::vector<handler_config> handler_list) {
    for (auto it = handler_list.begin(); it != handler_list.end(); it++) {
       request_handler_[it->location_prefix] = createHandler(it->type, it->location_prefix, it->config);
    }

    // it is kinda of ambiguous right now because we are inserting the two data into
    // handler_list.begin(). it could be wrong but should carefully look at it if there is an error
    // it will be stored into session & handle_dispatch
    request_handler_["/"] = not_found_handler::Init("/", handler_list.begin()->config);
}
