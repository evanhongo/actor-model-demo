#include <cstdint>

class Reactor;

class Handler
{
private:
    Reactor *reactor_;
    int client_fd_;

public:
    Handler(Reactor *reactor, int client_fd);
    int getClientFd();
    void handleEvent();
};