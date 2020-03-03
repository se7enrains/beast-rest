#include <iostream>
#include "server.h"


int main() {
    auto const address = "127.0.0.1";
    int const port = 8080;
    int threadsNumber = 1;

    Server server = Server(address, port, threadsNumber);

    return 0;
}
