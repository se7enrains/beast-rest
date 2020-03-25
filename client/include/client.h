//
// Created by se7enrains on 3/3/20.
//

#ifndef SIMPLE_REST_SERVICE_CLIENT_H
#define SIMPLE_REST_SERVICE_CLIENT_H

#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace beast = boost::beast;

class Client {
public:
    rapidjson::Document dom;
    Client(const std::string& ip, int port);
    void run();
private:
    using responseBodyType = beast::http::basic_dynamic_body<beast::flat_buffer>;

    std::string ip;
    int port;
    std::string sFilePath;
    std::string path;
    boost::asio::io_context ioContext;
    boost::asio::ip::tcp::socket* socket;
    beast::flat_buffer buffer;
    beast::http::request<beast::http::string_body> request;
    beast::http::response<responseBodyType> response;
    beast::http::response_parser<responseBodyType, std::allocator<char>> parser;


    void getFilePath();
    void connect();
    void prepareMessage();
    void sendMessage();
    void getResponse();
    void saveFile();
    void closeConnection();
};


#endif //SIMPLE_REST_SERVICE_CLIENT_H
