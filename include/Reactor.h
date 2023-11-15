#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/epoll.h>
#include <unordered_map>

#include "Acceptor.h"
#include "Handler.h"

using namespace std;

class Reactor
{
private:
    static constexpr int max_num_events = 1024;
    int listen_fd_;
    int epoll_fd_;
    unique_ptr<Acceptor> acceptor_;
    unordered_map<int, unique_ptr<Handler>> handlers_;

public:
    Reactor(int port);
    void registerHandler(Handler *handler);
    void removeHandler(Handler *handler);
    void dispatch(epoll_event event);
    void run();
};