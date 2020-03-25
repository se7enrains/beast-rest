//
// Created by se7enrains on 3/3/20.
//
#include "client.h"
#include "confighandler.h"

int main() {
    std::string path, ip, port;
    std::cout << "Enter path to config: ";
    std::cin >> path;
    ConfigHandler cfg(path);
    ip = cfg.getSetting("server.ip");
    port = cfg.getSetting("server.port");

    if(cfg.isOk()) {
        Client client(ip, std::stoi(port));
        client.run();
    } else {
        std::cout << cfg.getError() << std::endl;
    }
}