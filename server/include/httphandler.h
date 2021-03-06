//
// Created by se7enrains on 2/22/20.
//

#ifndef SIMPLE_REST_SERVICE_HTTPHANDLER_H
#define SIMPLE_REST_SERVICE_HTTPHANDLER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <iostream>
#include "../../extern/rapidjson/include/rapidjson/document.h"
#include "../../extern/rapidjson/include/rapidjson/stringbuffer.h"
#include "../../extern/rapidjson/include/rapidjson/writer.h"
#include <fstream>

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

class HttpHandler {
private:
    tcp::acceptor& acceptor;
    tcp::socket socket;
    boost::optional<beast::http::request_parser<beast::http::string_body, std::allocator<char>>> parser;
    beast::http::response<beast::http::string_body> response;
    boost::optional<beast::http::response_serializer<boost::beast::http::string_body>> responseSerializer;
    beast::multi_buffer buffer;

    void accept();
    void readRequest();
    void handleRequest(const beast::http::request<beast::http::string_body>& request);
    void sendBadRespononse(beast::http::status status,
                           const std::string& error);
public:
    HttpHandler(tcp::acceptor& acceptor);
    ~HttpHandler();
    void start();
};


#endif //SIMPLE_REST_SERVICE_HTTPHANDLER_H
