//
// Created by se7enrains on 2/22/20.
//
#include "server.h"

Server::Server(std::string ip, int port, int threadsNumber)
    : ip(std::move(ip)), port(port), threadsNumber(threadsNumber) { }

void Server::run() {
    asio::io_context ioContext;
    tcp::acceptor acceptor(ioContext,
            tcp::endpoint(asio::ip::address::from_string(ip), port));
    std::list<HttpHandler> handlers;
    for (int i = 0; i < threadsNumber; i++) {
        handlers.emplace_back(acceptor);
        handlers.back().start();
    }

    ioContext.run();
}
