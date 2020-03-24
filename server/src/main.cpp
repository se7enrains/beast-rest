#include <iostream>
#include "server.h"
#include "confighandler.h"


int main() {
    auto const address = "127.0.0.1";
    int const port = 37456;
    int threadsNumber = 1;

    Server server(address, port, threadsNumber);
    server.run();

    return 0;
}
