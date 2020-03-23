//
// Created by se7enrains on 3/3/20.
//
#include "client.h"

int main() {
    auto const address = "127.0.0.1";
    int const port = 37456;

    Client client(address, port);

    client.start();

}