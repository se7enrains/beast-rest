//
// Created by se7enrains on 3/24/20.
//


#ifndef SERVER_CONFIGHANDLER_H
#define SERVER_CONFIGHANDLER_H

#include <libconfig.h++>
#include <sstream>

using namespace libconfig;

class ConfigHandler {
private:
    Config cfg;
    bool ok;
    std::string error;
public:
    bool isOk();
    std::string getSetting(const std::string& setting);
    std::string getError();
    explicit ConfigHandler(const std::string& path);
    std::string getIp();
    std::string getPort();
};


#endif //SERVER_CONFIGHANDLER_H