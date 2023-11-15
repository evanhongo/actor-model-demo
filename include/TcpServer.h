#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <unordered_map>

class TcpServer
{
private:
    int fd_;
    Reactor *reactor_;

public:
    TcpServer();
    void start(int port);
};