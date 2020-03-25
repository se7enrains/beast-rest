#include <iostream>
#include "server.h"
#include "confighandler.h"


int main() {
    std::string path, ip, port, threadsNumber;
    std::cout << "Enter path to config: ";
    std::cin >> path;
    ConfigHandler cfg(path);
    ip = cfg.getSetting("server.ip");
    port = cfg.getSetting("server.port");
    threadsNumber = cfg.getSetting("server.threads");

    if(cfg.isOk()) {
        Server server(ip, std::stoi(port), std::stoi(threadsNumber));
        server.run();
    } else {
        std::cout << cfg.getError() << std::endl;
    }

    return 0;
}
