
#include "confighandler.h"

bool ConfigHandler::isOk() {
    return ok;
}

std::string ConfigHandler::getError() {
    return error;
}

ConfigHandler::ConfigHandler(const std::string& path) {
    try{
        cfg.readFile(path.c_str());
    }
    catch (const FileIOException &fioex){
        ok = false;
        error = "I/O error while reading file";
    }
    catch (const ParseException &pex){
        ok = false;
        std::stringstream ss;
        ss << "Parse exception at " << pex.getFile() << ":" << pex.getLine()
           << " - " << pex.getError();
        error = ss.str();
    }
    ok = true;
    error = "OK";
}

std::string ConfigHandler::getSetting(const std::string& setting) {
    if (ok) {
        try {
            return cfg.lookup("server.port").c_str();
        }
        catch (const SettingNotFoundException &snfex) {
            ok = false;
            error = std::string("No \"").append(setting).append("\" value in configuration file");
        }
    }
    return "";
}
