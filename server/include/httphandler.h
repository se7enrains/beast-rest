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
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <fstream>

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

class HttpHandler {
private:
    using requestBodyType = beast::http::basic_dynamic_body<beast::flat_buffer>;

    tcp::acceptor& acceptor;
    tcp::socket socket;
    boost::optional<beast::http::request_parser<requestBodyType, std::allocator<char>>> parser;
    beast::flat_buffer buffer;

    void accept();
    void readRequest();
    void handleRequest(const beast::http::request<requestBodyType>& request);
    void sendBadRespononse(beast::http::status status,
                           const std::string& error);
public:
    HttpHandler(tcp::acceptor& acceptor);
    void start();
};


#endif //SIMPLE_REST_SERVICE_HTTPHANDLER_H
