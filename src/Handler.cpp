#include "Handler.h"

Handler::Handler(Reactor *reactor, int client_fd) : reactor_(reactor), client_fd_(client_fd)
{
}

int Handler::getClientFd()
{
    return client_fd_;
}

void Handler::handleEvent()
{
}