#include "Reactor.h"

Reactor::Reactor(int port)
{
    listen_fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    epoll_fd_ = ::epoll_create1(EPOLL_CLOEXEC);
    acceptor_ = make_unique<Acceptor>(this, listen_fd_);

    if (listen_fd_ == -1)
    {
        perror("socket");
        return false;
    }

    int flags = fcntl(listen_fd_, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL");
        return false;
    }

    if (fcntl(listen_fd_, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl F_SETFL O_NONBLOCK");
        return false;
    }

    int reuseaddr = 1;
    if (setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1)
    {
        perror("setsockopt SO_REUSEADDR");
        return false;
    }

    int reuseport = 1;
    if (setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(int)) == -1)
    {
        perror("setsockopt SO_REUSEPORT");
        return false;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_fd_, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        return false;
    }

    if (listen(listen_fd_, 10) == -1)
    {
        perror("listen");
        return false;
    }

    addEpollCtl(listen_fd_, EPOLLET | EPOLLIN | EPOLLOUT);
}

Reactor::addEpollCtl(int fd, uint32_t events)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        perror("epoll_ctl EPOLL_CTL_ADD conn_fd");
        close(fd);
        return;
    }
}

Reactor::registerHandler(Handler *handler)
{
    int client_fd = handler->getClientFd();
    addEpollCtl(client_fd, EPOLLET | EPOLLIN | EPOLLRDHUP | EPOLLHUP);
    handlers_[client_fd] = handler;
}

Reactor::removeHandler(Handler *handler)
{
    int client_fd = handler->getClientFd();
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, nullptr);
    handlers.erase(client_fd);
    delete handler;
    close(client_fd);
}

Reactor::dispatch(epoll_event event)
{
    /* check if the connection is closing */
    if (event.events & (EPOLLRDHUP | EPOLLHUP))
    {
        Handler *handler = handlers_[event.data.fd];
        removeHandler(handler);
        printf("connection closed\n");
        return;
    }

    if (event.events & EPOLLIN)
    {
        if (event.data.fd == listen_fd_)
        {
            acceptor_->handleEvent();
            return;
        }

        Handler *handler = handlers_[event.data.fd];
        handler->handleEvent();
        return;
    }

    printf("unexpected\n");
}

Reactor::run()
{
    epoll_event events[max_num_events];
    while (true)
    {
        int num_events = epoll_wait(epoll_fd_, events, max_num_events, -1);
        for (int i = 0; i < num_events; i++)
            dispatch(events[i])
    }
}