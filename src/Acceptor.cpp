#include "Acceptor.h"

Acceptor::Acceptor(Reactor *reactor, int listen_fd) : reactor_(reactor), listen_fd_(listen_fd)
{
}

void Acceptor::handleEvent()
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = accept(listen_fd_, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0)
    {
        perror("accept");
        return;
    }

    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL");
        close(client_fd);
        return;
    }

    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl F_SETFL O_NONBLOCK");
        close(client_fd);
        return;
    }

    Handler *handler = new Handler(reactor_, client_fd);
    reactor_->registerHandler(handler);
}