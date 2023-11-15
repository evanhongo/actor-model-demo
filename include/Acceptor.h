#include <cstdint>
#include <fcntl.h>
#include <sys/socket.h>

#define DEFAULT_PORT 8080
#define MAX_CONN 16
#define MAX_EVENTS 32
#define BUF_SIZE 16
#define MAX_LINE 256

class Reactor;

class Acceptor
{
private:
    Reactor *reactor_;
    int listen_fd_;

public:
    Acceptor(Reactor *reactor, int listen_fd);
    void handleEvent();
};