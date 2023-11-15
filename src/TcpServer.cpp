#include "TcpServer.h"
#include "Reactor.h"

TcpServer::TcpServer()
{
}

void TcpServer::start(int port)
{
    reactor_ = new Reactor(port);
    reactor_.run();
}