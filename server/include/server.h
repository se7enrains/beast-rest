//
// Created by se7enrains on 2/22/20.
//

#ifndef SIMPLE_REST_SERVICE_SERVER_H
#define SIMPLE_REST_SERVICE_SERVER_H

#include <string>
#include <list>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "httphandler.h"

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

class Server {
private:
    std::string ip;
    int port;
    int threadsNumber;
public:
    Server(std::string ip, int port, int threadsNumber);
    void run();
};

#endif //SIMPLE_REST_SERVICE_SERVER_H
